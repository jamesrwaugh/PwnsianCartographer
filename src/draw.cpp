#include <SDL2/SDL.h>
#include "lodepng.h"
#include "utility.h"
#include "blocks.h"
#include "ChunkInterface.h"
#include "draw.h"

namespace draw
{

Drawer::Drawer()
{
    //TODO: Config file
    colors.load("items.zip", "items_color_cache.json");
}

SDL_Surface* Drawer::renderChunk(nbt_node* chunk)
{
    //Wrapper to tell us info about the ID at a position
    ChunkInterface iface(chunk);

    //SDL components to draw with
    SDL_Surface* surface = createRGBASurface(16, 16);
    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface);
    if(!surface || !renderer) {
        error("Cannot create rendering surface: ", SDL_GetError());
    }

    //This is where it all comes together!
    for(int z = 0; z != 16; ++z)
    for(int x = 0; x != 16; ++x)
    {
        //The data from the blocks
        unsigned id = iface.getHighestSolidBlockID(x,z);
        SDL_Color color = colors.getBlockColor(id);

        //Draw above color on image
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawPoint(renderer, x, z);
    }

    return surface;
}

SDL_Surface* Drawer::renderRegion(RegionFile& region)
{
    SDL_Surface* surface = createRGBASurface(16*32, 16*32);

    for(const auto& pair : region.getAllChunks())
    {
        //Render the chunk
        SDL_Surface* chunk = renderChunk(pair.second);

        //Put the chunk surface into the main surface, stitching the chunks together
        SDL_Rect blitdst = {pair.first.x*16, pair.first.z*16, 16, 16};
        SDL_BlitSurface(chunk, nullptr, surface, &blitdst);

        //We're done with this
        SDL_FreeSurface(chunk);
    }

    return surface;
}

SDL_Surface* Drawer::renderWorld(RegionFileWorld& world)
{
    //Width of a region in blocks
    const int size = 32*16;

    //Find the lowest X and Z coordinates for proper offset
    MC_Point offset = topleftOffset(world);

    /* Then, create an image of the size of the world in blocks
     * to place all of the region renders on */
    MC_Point worldSize = world.getSize();
    SDL_Surface* surface = createRGBASurface(worldSize.x, worldSize.z);

    for(auto& pair : world.getAllRegions())
    {
        //The rendered region
        SDL_Surface* region = renderRegion(pair.second);

        //Put the region surface into the main surface, stitching the regions together
        SDL_Rect blitdst = { (offset.x + pair.first.x)*size,
                             (offset.z + pair.first.z)*size, size, size };
        SDL_BlitSurface(region, nullptr, surface, &blitdst);

        //We're done with this
        SDL_FreeSurface(region);
    }

    drawGirdLines(surface);

    return surface;
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

bool saveSurfacePNG(SDL_Surface* surface, const std::string& filename)
{
    int w = surface->w;
    int h = surface->h;
    return lodepng::encode(filename, (unsigned char*)surface->pixels, w, h) == 0;
}

}
