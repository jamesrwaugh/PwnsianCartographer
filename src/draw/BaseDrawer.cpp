#include <SDL2/SDL.h>
#include "blocks/blocks.h"
#include "anvil/ChunkInterface.h"
#include "utility/utility.h"
#include "utility/lodepng.h"
#include "maginatics/threadpool/threadpool.h"
#include "draw/BaseDrawer.h"

namespace draw
{

SDL_Surface* BaseDrawer::renderWorld(RegionFileWorld& world, const arguments::Args& options)
{
    //Virtual call
    recieveArguments(options);        

    //Find the lowest X and Z coordinates for proper offset
    MC_Point offset = getTopleftOffset(world);

    MC_Point worldSize = world.getSize();
    SDL_Surface* surface = createRGBASurface(worldSize.x * scale, worldSize.z * scale);

    //Keeping track of threads in a pool
    maginatics::ThreadPool pool(1, maxThreads, 30);

    for(auto& pair : world.getAllRegions())
    {
        //Location to render the region
        int x = (pair.first.x + offset.x) * regionsize;
        int z = (pair.first.z + offset.z) * regionsize;

        /* Bind the "renderRegion" member function, to call in a thread.
         * Somewhere deep inside std::bind, the copy ctor of RegionFile is called,
         * so renderRegion needs to accept a RegionFile pointer instead. (&pair.second) */
        auto function = std::bind(&BaseDrawer::renderRegion, this, MC_Point{x,z}, surface, &pair.second);

        //Queue a new thread to render this region
        pool.execute(function);
    }

    pool.drain();

    //Add cool grid lines
    if(gridlines) {
        drawGirdLines(surface);
    }

    return surface;
}

SDL_Surface* BaseDrawer::renderWorld(const std::string& filename, const arguments::Args& options)
{
    RegionFileWorld world(filename);
    return renderWorld(world, options);
}

/* Render a single region to an existing surface.
 * The renderer is created in this function to ensure one renderer per thread.
 * They're all just spitting things out to the same surface */
void BaseDrawer::renderRegion(MC_Point location,  SDL_Surface* surface, RegionFile* region)
{
    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface);
    SDL_RenderSetScale(renderer, scale, scale);

    for(const auto& pair : region->getAllChunks())
    {
        //The draw location is: region location + chunk location
        int x = location.x + pair.first.x*16;
        int z = location.z + pair.first.z*16;

        renderChunk(MC_Point{x,z}, renderer, pair.second);
    }

    SDL_DestroyRenderer(renderer);
}

void BaseDrawer::renderChunk(MC_Point location,
                         SDL_Renderer* renderer,
                         nbt_node *chunk)
{
    //Wrapper to tell us info about the ID at a position
    ChunkInterface iface(chunk);

    //This is where it all comes together!
    for(int z = 0; z != 16; ++z)
    for(int x = 0; x != 16; ++x)
    {
    	//Virtual call
        SDL_Color color = renderBlock(iface, x, z);

        //Draw above color on image
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawPoint(renderer, location.x + x, location.z + z);
    }
}

MC_Point BaseDrawer::getTopleftOffset(RegionFileWorld& world)
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

void BaseDrawer::drawGirdLines(SDL_Surface* s)
{
    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(s);
    SDL_RenderSetScale(renderer, scale, scale);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    //Vertical lines
    for(int x = 0; x < s->w; x += regionsize)
        SDL_RenderDrawLine(renderer, x, 0, x, s->h);

    //Horizontal lines
    for(int y = 0; y < s->h; y += regionsize)
        SDL_RenderDrawLine(renderer, 0, y, s->w, y);
        
    SDL_DestroyRenderer(renderer);
}

SDL_Surface* BaseDrawer::createRGBASurface(int w, int h)
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

void BaseDrawer::recieveArguments(const arguments::Args& options)
{
    //Max drawing threads
    maxThreads = options.numThreads;
    
    //The scale of the image
    scale = options.scale;

    //Draw gridlines
    gridlines = options.gridlines;
}

}
