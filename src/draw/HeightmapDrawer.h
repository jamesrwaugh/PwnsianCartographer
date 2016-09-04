#ifndef HEIGHTMAP_DRAWER_H
#define HEIGHTMAP_DRAWER_H
#include "draw/BaseDrawer.h"

/* HeightmapDrawer is a drawer that will output a color based
 * on how high each block is */

namespace draw
{

class HeightmapDrawer : public BaseDrawer
{
protected:
    SDL_Color renderBlock(ChunkInterface& iface, int x, int z) override;

private:
    SDL_Color hsv2rgb(float h, float s, float v);
};

}

#endif
