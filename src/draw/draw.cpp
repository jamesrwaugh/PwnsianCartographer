#include <SDL2/SDL.h>
#include "utility/utility.h"
#include "utility/savepng.h"
#include "utility/lodepng.h"
#include "draw/draw.h"

namespace draw
{

/* Drawer registration
 * Drawers are enabled by adding them to the drawerRegistry object.
 * The map related a drawer type to a name, and a function that, when called,
 * returns and instance of the drawer. */

//A function that creates a drawer
typedef std::function<std::unique_ptr<BaseDrawer>(void)> DrawerCreatorFn;

//Small type to store name and creation information about a drawer
struct DrawerRegistry
{
public:
    //Main function used to create a DrawerRegistry instance.
    //The reason is because DrawerRegistry cannot be a template object
    //itself in order to fit into the drawerRegistry map
    template<typename DrawerType>
    static DrawerRegistry create(const std::string& name) 
    {
        return DrawerRegistry { name, GetDrawerCreatorFn<DrawerType>() };
    }

public:
    //User-friendly Name of drawer, specified on command line
    std::string name;

    //Function to create an instance of this drawer
    DrawerCreatorFn creator;

private:
    //Helper to return a function that returns an instance of a drawer
    // Used to assign to DrawerRegistry::creator
    template<typename DrawType>
    static constexpr DrawerCreatorFn GetDrawerCreatorFn()
    {
        return [](){ return std::make_unique<DrawType>(); };
    }
};

//Main object to register a drawer. 
static const std::map<DrawerType, DrawerRegistry> drawerRegistry = 
{
    { DrawerType::Normal,    DrawerRegistry::create<NormalDrawer>("normal")    },
    { DrawerType::HeightMap, DrawerRegistry::create<HeightmapDrawer>("height") },
    { DrawerType::Shaded,    DrawerRegistry::create<ShadedDrawer>("shaded")    }       
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
        return it->second.creator();
    }

    //Exception - should not happen
    error("Invalid drawer type \"", int(type));    
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
