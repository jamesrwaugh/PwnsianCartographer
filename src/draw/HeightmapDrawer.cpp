#include "utility/utility.h"
#include "draw/HeightmapDrawer.h"

namespace draw
{

SDL_Color HeightmapDrawer::renderBlock(ChunkInterface& iface, int x, int z)
{
    //Right-most hue on the HSV scale to consider any block
    const int MAX_HUE = 180;
    //Minecraft height we consider maximum
    const int MAX_MC_HEIGHT = 128;

    Uint8 hightestY = iface.getHighestSolidBlockY(x, z);

    // 0..MAX_MC_HEIGHT scaled to 0..MAX_HUE (HSV hue range)
    // "MAX_HUE - hue" is used to go from the center of the HSV scale to the left
    // (light blue to red). See a HSV hue diagram for colors
    int hue = MAX_HUE - ((float)hightestY / MAX_MC_HEIGHT) * MAX_HUE;
    hue = clamp(hue, 0, MAX_HUE);

    //Cache hue colors for retrieval
    if(colorCache[hue].a == SDL_ALPHA_TRANSPARENT) {
        colorCache[hue] = hsv2rgb(hue, 1.0, 1.0);
    }

    return colorCache[hue];
}

//This forumla can be found on Wikipedia or similar, searching for "hsv to rgb"
SDL_Color HeightmapDrawer::hsv2rgb(float h, float s, float v)
{
    double      hh, p, q, t, ff;
    long        i;
    SDL_Color   out;

    if(s <= 0.0) {
        out.r = v;
        out.g = v;
        out.b = v;
        return out;
    }

    hh = h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = v * (1.0 - s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));

    //float 0..1 range to byte 0..255 range, for SDL_Color
    v *= 255; t *= 255; p *= 255; q *= 255;

    switch(i)
    {
    case 0:  out.r = v; out.g = t; out.b = p; break;
    case 1:  out.r = q; out.g = v; out.b = p; break;
    case 2:  out.r = p; out.g = v; out.b = t; break;
    case 3:  out.r = p; out.g = q; out.b = v; break;
    case 4:  out.r = t; out.g = p; out.b = v; break;
    default: out.r = v; out.g = p; out.b = q; break;
    }

    //Fully opaque output
    out.a = SDL_ALPHA_OPAQUE;

    return out;
}

}

