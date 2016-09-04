#include <SDL2/SDL.h>
#include "utility/utility.h"
#include "utility/savepng.h"
#include "utility/lodepng.h"
#include "draw/draw.h"

namespace draw
{

std::unique_ptr<BaseDrawer> createDrawer(DrawerType type)
{
    switch(type)
    {
    case DrawerType::Normal:
        return std::make_unique<NormalDrawer>();
        break;
    case DrawerType::HeightMap:
        return std::make_unique<HeightmapDrawer>();
        break;
    default:
        error("Invalid drawer type \"", int(type), "\"");
    }
}

//Generic useful function to combine all keys of a map
template<typename Key, typename Value>
std::string getAllKeys(const std::map<Key,Value>& map)
{
    std::string results;
    for(const auto& pair : map) {
        results += pair.first + " ";
    }
    results.pop_back(); //Remove trailing space
    return results;
}

DrawerType getDrawerType(const std::string &drawerName)
{
    static const std::map<std::string, DrawerType> drawers = {
        {"normal", DrawerType::Normal},
        {"height", DrawerType::HeightMap}
    };
    auto it = drawers.find(drawerName);
    if(it != drawers.end()) {
        return it->second;
    }

    //Exception
    error("Invalid drawer type \"", drawerName,"\". Valid: ", getAllKeys(drawers));
}

/* On Windows, use a lodepng (no extra dependencies). *nix, use libpng */
bool saveSurfacePNG(SDL_Surface* surface, const std::string& filename)
{
#ifdef __WINDOWS__
    int w = surface->w;
    int h = surface->h;
    return lodepng::encode(filename, (unsigned char*)surface->pixels, w, h) == 0;
#else
    int success =  SDL_SavePNG(surface, filename.c_str());
    if(success != 0) {
        error("Could not save PNG: ", SDL_GetError());
    }
#endif
}

}
