#ifndef DRAW_H
#define DRAW_H

#include <array>
#include <vector>
#include "types.h"
#include "blocks.h"
#include "AnvilFile.h"
#include "AnvilFileWorld.h"
#include "nbt/nbt.h"

/* Rendering section of code. Contains Drawer, which is a classed used
 * to render chunks, regons, and entire worlds to SDL_Surfaces. */

namespace draw
{

/* Main world rendering class */

class Drawer
{
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

    //Put region-sized (32x32) gridlines on a surface
    void drawGirdLines(SDL_Surface* s);

    /* This gives us the coordinates of the upper-left region in the world.
     * i.e, the one the least x and Z positions, such as r.-3.-4.mca
     * This is used to properly size the final drawing, and draw all regions
     * relative to the top-left of the image */
    MC_Point topleftOffset(RegionFileWorld& world);

    //Item to get colors based on block IDs
    blocks::BlockColors colors;
};

}

namespace draw
{

/* Generic helper function to save a surface to a PNG at "filename" */
bool saveSurfacePNG(SDL_Surface* surface, const std::string& filename);

}

#endif // DRAW_H
