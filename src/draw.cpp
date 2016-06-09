#include <SDL2/SDL.h>
#include <future>
#include "config.h"
#include "utility.h"
#include "blocks.h"
#include "ChunkInterface.h"
#include "savepng.h"
#include "lodepng.h"
#include "draw.h"

namespace draw
{

Drawer::Drawer()
{
    //Load the BlockColors to retrive color info from
    colors.load(config::get().GetString("items-zip-name"),
                config::get().GetString("items-cache-name"));

    //Max drawing threads; < 0 means use number of cores aviliable
    int threads = config::get().GetInt("drawer-max-threads");
    if(threads < 0) {
        maxThreads = SDL_GetCPUCount();
    } else {
        maxThreads = threads;
    }

    //The scale of the image. Minimum is 1
    scale = config::get().GetInt("drawer-scale");
    scale = std::max(1u, scale);
}

SDL_Surface* Drawer::renderWorld(RegionFileWorld& world)
{
    //Find the lowest X and Z coordinates for proper offset
    MC_Point offset = topleftOffset(world);

    /* Then, create an image of the size of the world in blocks
     * to place all of the region renders on */
    MC_Point worldSize = world.getSize();
    SDL_Surface* surface = createRGBASurface(worldSize.x * scale, worldSize.z * scale);

    //Keeping track of threads
    std::vector<std::future<void>> threads;

    for(auto& pair : world.getAllRegions())
    {
        //Location to render the region
        int x = (pair.first.x + offset.x) * regionsize;
        int z = (pair.first.z + offset.z) * regionsize;

        /* Bind the "renderRegion" member function, to call in a thread.
         * Somewhere deep inside std::bind/async, the copy ctor of RegionFile is called,
         * so renderRegion needs to accept a RegionFile pointer instead. (&pair.second) */
        auto function = std::bind(&Drawer::renderRegion, this,
                                  MC_Point{x,z},
                                  surface,
                                  &pair.second);

        //Launch a new thread to render this region
        threads.push_back(std::async(std::launch::async, function));

        //If we've got a maximum number of threads, wait for them all
        if(threads.size() >= maxThreads) {
            for(auto& thread : threads) {
                thread.get();
            }
            threads.clear();
        }
    }

    //Catches the case in which the for-loop finishes with less than max threads
    for(auto& thread : threads) {
        thread.get();
    }

    //Add cool grid lines
    if(config::get().GetInt("drawer-draw-gridlines")) {
        drawGirdLines(surface);
    }

    return surface;
}

/* Render a single region to an existing surface.
 * The renderer is created in this function to ensure one renderer per thread.
 * They're all just spitting things out to the same surface */
void Drawer::renderRegion(MC_Point location,  SDL_Surface* surface, RegionFile* region)
{
    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface);
    SDL_RenderSetScale(renderer, scale, scale);

    for(const auto& pair : region->getAllChunks())
    {
        //The draw location is the passed in region location + chunk location
        int x = location.x + pair.first.x*16;
        int z = location.z + pair.first.z*16;

        renderChunk(MC_Point{x,z}, renderer, pair.second);
    }

    SDL_DestroyRenderer(renderer);
}

void Drawer::renderChunk(MC_Point location,
                         SDL_Renderer* renderer,
                         nbt_node *chunk)
{
    //Wrapper to tell us info about the ID at a position
    ChunkInterface iface(chunk);

    //This is where it all comes together!
    for(int z = 0; z != 16; ++z)
    for(int x = 0; x != 16; ++x)
    {
        //The data from the blocks
        blocks::BlockID id = iface.getHighestSolidBlockID(x,z);
        SDL_Color color = colors.getBlockColor(id);

        //Draw above color on image
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawPoint(renderer, location.x + x, location.z + z);
    }
}

MC_Point Drawer::topleftOffset(RegionFileWorld& world)
{
    /* We need to find the regions with the lowest X, and Z.
     * They may not nessecerily be the same region. These X and Z
     * are combined to form the upper-left corner of the render */
    int lowestX = 0, lowestZ = 0;

    for(auto& region : world.getAllRegions())
    {
        int x = region.first.x;
        int z = region.first.z;

        lowestX = std::min(lowestX, x);
        lowestZ = std::min(lowestZ, z);
    }

    /* Absolute value is used to push other reigons down and to the right,
     * because otherwise a negative value would push them up and to the left.
     * Basically, draw.X = lowestX + region.X */
    return { abs(lowestX), abs(lowestZ) };
}

void Drawer::drawGirdLines(SDL_Surface* s)
{
    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(s);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    //Vertical lines
    for(int x = 0; x < s->w; x += 32*16)
        SDL_RenderDrawLine(renderer, x, 0, x, s->h);

    //Horizontal lines
    for(int y = 0; y < s->h; y += 32*16)
        SDL_RenderDrawLine(renderer, 0, y, s->w, y);
}

SDL_Surface* Drawer::createRGBASurface(int w, int h)
{
    SDL_Surface* surface =  SDL_CreateRGBSurface(0, w, h, 32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
#else
    0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
#endif
    );

    if(!surface) {
        error("Cannot create rendering surface: ", SDL_GetError());
    }

    return surface;
}

}

namespace draw
{

/* On Windows, use a lodepng (no extra dependencies). *nix, use libpng */

bool saveSurfacePNG(SDL_Surface* surface, const std::string& filename)
{
#ifdef __WINDOWS__
    int w = surface->w;
    int h = surface->h;
    return lodepng::encode(filename, (unsigned char*)surface->pixels, w, h) == 0;
#else
    return SDL_SavePNG(surface, filename.c_str());
#endif
}

}
