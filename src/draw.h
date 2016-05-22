#ifndef DRAW_H
#define DRAW_H

#include <array>
#include <vector>

#include "types.h"
#include "blocks.h"
#include "AnvilFile.h"
#include "AnvilFileWorld.h"
#include "nbt/nbt.h"

/* Routines to draw colors based on an NBT chunk node */

namespace draw
{

class Drawer
{
public:
    /* Generic public helper function to save a surface to a PNG given by
     * "filename" */
    static bool saveSurfacePNG(SDL_Surface* surface, const std::string& filename);

public:
    Drawer();

    //Render a single chunk to a surface
    SDL_Surface* renderChunk(nbt_node* chunk);

    //Render a single region to a surface
    SDL_Surface* renderRegion(RegionFile& region);

    /* Renders a world, returns an RGBA SDL_Surface of the image.
     * Can be saved by lodepng::encode on (unsigned char*)surface->pixels */
    SDL_Surface* renderWorld(RegionFileWorld &world);

private:
    /* Utility to create a 32-bit RGBA surface with w/h,
     * taking endianness into account */
    SDL_Surface* createRGBASurface(int w, int h);

    //Item to get colors based on block IDs
    blocks::BlockColors colors;
};

}

#endif // DRAW_H
