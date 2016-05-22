#include <SDL2/SDL.h>
#include "blocks.h"
#include "ChunkInterface.h"
#include "draw.h"

//Low-level functions
// ============================================================================
namespace draw
{

void renderChunk(nbt_node* chunk, ChunkRender& out)
{
    //Wrapper to tell us info about the ID at a position
    ChunkInterface iface(chunk);

    //Item to get colors for a given ID
    blocks::BlockColors colors;
    colors.load("items.zip", "items_color_cache.json");

    //256 RGBA pixels
    out.resize(16*16*4);

    //This is where it all comes together!
    for(int z = 0; z != 16; ++z)
    for(int x = 0; x != 16; ++x)
    {
        unsigned id = iface.getHighestSolidBlockID(x,z);
        //unsigned id = 2;
        SDL_Color color = colors.getBlockColor(id);

        /* The 4* accounts for there being 4 places in the array
         * for each color. */
        unsigned index = 4*(16*z + x);
        out[index + 0] = color.r;
        out[index + 1] = color.g;
        out[index + 2] = color.b;
        out[index + 3] = color.a;
    }
}

}

//High-level function
// ============================================================================
namespace draw
{

bool renderWorld(RegionFileWorld&, png&)
{
    return false;
}

}
