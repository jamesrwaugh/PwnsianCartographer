#include "draw/HeightmapDrawer.h"

namespace draw
{

SDL_Color HeightmapDrawer::renderBlock(ChunkInterface& iface, int x, int z)
{
    Uint8 hightestY = iface.getHighestSolidBlockY(x, z);

    // 0..128 (Minecraft height) scaled to 0..180 (HSV hue range)
    int h = ((float)hightestY / 128) * 180;

    return hsv2rgb(180 - h, 1.0, 1.0);
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

