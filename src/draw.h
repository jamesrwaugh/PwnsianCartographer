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
typedef std::vector<byte> png;

/* Renders a world, and returns a lodePNG in out.
 * It should be saved directly by lodepng::savefile or similar */
bool renderWorld(const RegionFileWorld& world, png& out);

}

#endif // DRAW_H
