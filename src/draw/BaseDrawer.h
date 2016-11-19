#ifndef BASEDRAWER_H
#define BASEDRAWER_H
#include <vector>
#include "types.h"
#include "anvil/ChunkInterface.h"
#include "blocks/blocks.h"
#include "anvil/RegionFile.h"
#include "anvil/RegionFileWorld.h"
#include "utility/arguments.h"
#include "nbt/nbt.h"

/* Abstract base renderer class. BaseDrawer handles threads, image stitching,
 * gridlines, regions, and everything else. Child classes provide how to render a block
 * (renderBlock) which BaseDrawer places on the final images */

namespace draw
{

/* Main world rendering class */

class BaseDrawer : arguments::ArgumentReciever
{
public:
    /* Renders a world (loaded or from file); returns an RGBA SDL_Surface of the image.
     * "options" are entered from the command line, to be used for drawing options.
     * Can be saved by draw::saveSurfacePNG (or your method) */
    SDL_Surface* renderWorld(RegionFileWorld& world, const arguments::Args& options);
    SDL_Surface* renderWorld(const std::string& filename, const arguments::Args& options);

protected:
    void recieveArguments(const arguments::Args& args) override;
    virtual SDL_Color renderBlock(ChunkInterface& iface, int x, int z) = 0;

private:
    //Width of a region in blocks
    static const int regionsize = 32*16;

    //The maximum number of thread to use when rendering
    //A value of 0 will spawn 1 thread, though
    unsigned maxThreads = 1;
    //The scale of the image. 1x is 1-pixel-per-block, 2x is a 2x2 square per block...
    unsigned scale = 1;
    //Draw gridline options
    bool gridlines = false;

    //Render a single chunk to an existing surface
    void renderChunk(MC_Point location, SDL_Renderer *renderer, nbt_node* chunk);

    //Render a single region to an existing surface at "location" XY
    void renderRegion(MC_Point location, SDL_Surface* surface, RegionFile* region);

    /* Ccreate a 32-bit RGBA surface taking endianness into account */
    SDL_Surface* createRGBASurface(int w, int h);

    //Put region-sized (512x512) gridlines on a surface
    void drawGirdLines(SDL_Surface* s);

    /* This gives us the magnitude of left-most (-X) and top-most (-Z) regions,
     * e.g.: A world has r.-3.-2.mca and r.0.-4.mca -> {3,4}
     * This is used to properly place the final drawing, by "pushing" all regions
     * by that offset, relative to the top-left of the image */
    MC_Point getTopleftOffset(RegionFileWorld& world);
};

}

#endif
