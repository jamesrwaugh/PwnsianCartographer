#ifndef DRAW_H
#define DRAW_H
#include <vector>

#include "types.h"
#include "blocks/blocks.h"
#include "anvil/RegionFile.h"
#include "anvil/RegionFileWorld.h"
#include "nbt/nbt.h"

/* Rendering section of code. Contains Drawer, which is a class used
 * to render chunks, regons, and entire worlds to SDL_Surfaces. */

namespace draw
{

/* Main world rendering class */

class Drawer
{
public:
    Drawer();

    /* Renders a world, returns an RGBA SDL_Surface of the image.
     * Can be saved by draw::saveSurfacePNG (or your method) */
    SDL_Surface* renderWorld(RegionFileWorld& world);

private:
    //Width of a region in blocks
    static const int regionsize = 32*16;

    //The maximum number of thread to use when rendering
    //A value of 0 will spawn 1 thread, though
    unsigned maxThreads = 1;

    //The scale of the image. 1x is 1-pixel-per-block, 2x is a 2x2 square per block...
    unsigned scale = 1;

    //Render a single chunk to an existing surface
    void renderChunk(MC_Point location, SDL_Renderer *renderer, nbt_node* chunk);

    //Render a single region to an existing surface at "location" XY
    void renderRegion(MC_Point location, SDL_Surface* surface, RegionFile* region);

    /* Utility to create a 32-bit RGBA surface with w/h,
     * taking endianness into account */
    SDL_Surface* createRGBASurface(int w, int h);

    //Put region-sized (512x512) gridlines on a surface
    void drawGirdLines(SDL_Surface* s);

    /* This gives us the magnitude of left-most (-X) and top-most (-Z) regions,
     * e.g.: World has r.-3.-2.mca and r.0.-4.mca -> {3,4}
     * This is used to properly place the final drawing, by "pushing" all regions
     * by that offset, relative to the top-left of the image */
    MC_Point topleftOffset(RegionFileWorld& world);

    //Item to get colors based on block IDs
    blocks::BlockColors colors;
};

}

namespace draw
{

/* Generic helper function to save a surface to a PNG at "filename"
 * return true on success */
bool saveSurfacePNG(SDL_Surface* surface, const std::string& filename);

}

#endif // DRAW_H
