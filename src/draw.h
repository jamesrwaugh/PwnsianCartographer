#ifndef DRAW_H
#define DRAW_H

#include <array>
#include <vector>

#include "types.h"
#include "AnvilFile.h"
#include "AnvilFileWorld.h"
#include "nbt/nbt.h"

/* Routines to draw colors based on an NBT chunk node */

namespace draw
{

//Type represneting a lodePNG image
typedef std::vector<unsigned char> png;

/* 16x16 array of RGBA pixels, render of a chunk.
 * Vector size: 16x16x4 */
typedef std::vector<unsigned char> ChunkRender;

}


namespace draw
{

//Temp exposure: Render a single chunk
void renderChunk(nbt_node* chunk, ChunkRender& out);

/* Renders a world, and returns a lodePNG in out.
 * It should be saved directly by lodepng::savefile or similar */
bool renderWorld(const RegionFileWorld& world, png& out);

}

#endif // DRAW_H
