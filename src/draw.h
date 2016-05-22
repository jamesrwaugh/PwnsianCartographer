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

/* 512x512 array of RGBA pixels, render of a region (32x32 chunks)
 * Vector size: 512x512x4 */
typedef std::vector<unsigned char> RegionRender;

}

namespace draw
{

//Temp exposure: Render a single chunk
SDL_Surface* renderChunk(nbt_node* chunk);

//Temp exposure: Render a single region
SDL_Surface* renderRegion(RegionFile& region);

/* Renders a world, returns an RGBA SDL_Surface of the image.
 * CAn be saved by lodepng::encode on (unsigned char*)surface->pixels */
SDL_Surface* renderWorld(const RegionFileWorld& world);

}

#endif // DRAW_H
