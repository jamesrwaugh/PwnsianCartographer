#include "utility/utility.h"
#include "draw/ShadedDrawer.h"

namespace draw
{

SDL_Color ShadedDrawer::renderBlock(ChunkInterface& iface, int x, int z)
{
    //Height of sea level. "middle" height of a map
    const int MC_ABOVE_SEA_LEVEL = 80;

    SDL_Color color = NormalDrawer::renderBlock(iface, x, z);
    int blockY = iface.getHighestSolidBlockY(x, z);

    //A value, 0 to 1.1, which multiplies each RGB component
    //to obtain a ligher/darker color
    float shadePercent = (float)blockY / MC_ABOVE_SEA_LEVEL;
    shadePercent = clamp(shadePercent, 0.f, 1.1f);

    color.r = clamp(color.r * shadePercent, 0.f, 255.f);
    color.g = clamp(color.g * shadePercent, 0.f, 255.f);
    color.b = clamp(color.b * shadePercent, 0.f, 255.f);

    return color;
}

}