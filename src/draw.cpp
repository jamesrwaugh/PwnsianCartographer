#include <SDL2/SDL.h>
#include "lodepng.h"
#include "utility.h"
#include "blocks.h"
#include "ChunkInterface.h"
#include "draw.h"

//Low-level functions
// ============================================================================
namespace draw
{

Drawer::Drawer()
{
    //TODO: Config file
    colors.load("items.zip", "items_color_cache.json");
}

bool Drawer::saveSurfacePNG(SDL_Surface* surface, const std::string& filename)
{
    int w = surface->w;
    int h = surface->h;
    return lodepng::encode(filename, (unsigned char*)surface->pixels, w, h) == 0;
}

SDL_Surface* Drawer::renderChunk(nbt_node* chunk)
{
    //Wrapper to tell us info about the ID at a position
    ChunkInterface iface(chunk);

    //SDL components to draw TODO: Put in class to avoid recreation
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

        //Draw above color on blocks
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
        SDL_Rect blitdst = {pair.first.first*16, pair.first.second*16, 16, 16};
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
    SDL_Point worldSize = world.getSize();
    SDL_Point center = {3000, 3000};

    log("Size:", worldSize.x, " " , worldSize.y);

    SDL_Surface* surface = createRGBASurface(6000, 6000);

    for(auto& pair : world.getAllRegions())
    {
        SDL_Surface* region = renderRegion(pair.second);

        //Put the region surface into the main surface, stitching the regions togeher
        SDL_Rect blitdst = {center.x + pair.first.first*size,
                            center.x + pair.first.second*size, size, size};
        SDL_BlitSurface(region, nullptr, surface, &blitdst);

        //We're done with this
        SDL_FreeSurface(region);
    }

    return surface;
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
