#include <SDL2/SDL.h>
#include "utility/utility.h"
#include "utility/savepng.h"
#include "utility/lodepng.h"
#include "draw/draw.h"

namespace draw
{

/* Drawer registration
 * Drawers are enabled by adding them to the drawerRegistry object.
 * It relates a type enum to DrawerRegistry object, which contains
 * a name and a creation function (returns unique_ptr) */

//A function that creates a drawer
typedef std::function<std::unique_ptr<BaseDrawer>(void)> DrawerCreatorFn;

struct DrawerRegistry
{
    std::string name;
    DrawerCreatorFn create;
};

template<typename DrawerType>
DrawerRegistry makeDrawerRegistry(const std::string& name)
{
    return DrawerRegistry {
        name,
        []() { return std::make_unique<DrawerType>(); }
    };
}

//Main object to register a drawer. 
static const std::map<DrawerType, DrawerRegistry> drawerRegistry = 
{
    { DrawerType::Normal,    makeDrawerRegistry<NormalDrawer>("normal")    },
    { DrawerType::HeightMap, makeDrawerRegistry<HeightmapDrawer>("height") },
    { DrawerType::Shaded,    makeDrawerRegistry<ShadedDrawer>("shaded")    }       
};

/* ------------------------------------------------------------------------- */

std::string getAllDrawerNames()
{
    std::string result;
    for(const auto& pair : drawerRegistry)  {
        result += pair.second.name + " ";
    }
    result.pop_back(); //Remove trailing space
    return result;
}

DrawerCreatorFn::result_type createDrawer(DrawerType type)
{
    auto it = drawerRegistry.find(type);
    if(it != drawerRegistry.end()) {
        return it->second.create();
    }

    //Exception - should not happen
    error("Invalid drawer type \"", int(type), "\"");    
}

DrawerType getDrawerType(const std::string& drawerName)
{
    for(const auto& pair : drawerRegistry)  {
        if(pair.second.name == drawerName) {
            return pair.first;
        }
    }

    //Exception
    error("Invalid drawer \"", drawerName + "\". Valid: ", getAllDrawerNames());
}

std::string getDrawerName(const DrawerType& type)
{
    auto it = drawerRegistry.find(type);
    if(it != drawerRegistry.end()) {
        return it->second.name;
    }    

    //Exception - should never happen
    error("Invalid drawer type \"", int(type));    
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
