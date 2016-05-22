#include <SDL2/SDL.h>
#include "draw.h"

//Type definitions
// ============================================================================
namespace draw
{

struct pixel
{
    union {
        byte r, g, b, a;
        int value;
    };
};

//16x16 array of pixels, render of a chunk
typedef std::array<std::array<pixel,16>,16> ChunkRender;

//PNG datatype
typedef std::vector<unsigned char> png;

}

//Low-level functions
// ============================================================================
namespace draw
{

bool renderChunk(const nbt_node* chunk, ChunkRender& out)
{
    //We need to look in the HeightMap to tell us where the highed solid block is


    //Then, take the last 16x16 blocks off the back
}

}

//High-level function
// ============================================================================
namespace draw
{

bool renderWorld(RegionFileWorld& world, png& out)
{
    for(auto& pair : world.regionMap())
    {

    }
}

}
