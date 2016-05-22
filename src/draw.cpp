#include <SDL2/SDL.h>
#include "utility.h"
#include "blocks.h"
#include "ChunkInterface.h"
#include "draw.h"

//Low-level functions
// ============================================================================
namespace draw
{

SDL_Surface* createRGBASurface(int w, int h)
{
    return SDL_CreateRGBSurface(0, w, h, 32,
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
            0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
        #else
            0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
        #endif
        );
}

SDL_Surface* renderChunk(nbt_node* chunk)
{
    //Wrapper to tell us info about the ID at a position
    ChunkInterface iface(chunk);

    //Item to get colors for a given ID
    blocks::BlockColors colors;
    colors.load("items.zip", "items_color_cache.json");

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

SDL_Surface* renderRegion(RegionFile& region)
{
    //The center of the image in X/Y pixels
    int center = 0;

    SDL_Surface* surface = createRGBASurface(16*32, 16*32);
    if(!surface) {
        error("Cannot create rendering surface: ", SDL_GetError());
    }

    for(const auto& pair : region.getAllChunks())
    {
        //Render the chunk
        SDL_Surface* chunk = renderChunk(pair.second);

        //Put the chunk surface into the main surface, stitching the chunks together
        SDL_Rect blitdst = {center + pair.first.first*16, center + pair.first.second*16, 16, 16};
        SDL_BlitSurface(chunk, nullptr, surface, &blitdst);
    }

    return surface;
}

}

//High-level function
// ============================================================================
namespace draw
{

SDL_Surface* renderWorld(const RegionFileWorld&)
{
    return nullptr;
}

}
