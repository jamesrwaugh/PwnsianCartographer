#ifndef DRAW_H
#define DRAW_H
#include "draw/BaseDrawer.h"
#include "draw/NormalDrawer.h"
#include "draw/HeightmapDrawer.h"
#include "draw/ShadedDrawer.h"

/* Top-level draw include file. */

namespace draw
{

//Applicable drawer types
enum class DrawerType
{
    Normal = 0,
    HeightMap,
    Shaded
};

/* Returns a new instance of a drawer based on type */
std::unique_ptr<BaseDrawer> createDrawer(DrawerType type);

/* Return drawer type based on name, eg "normal" or "height".
 * This is primarily used for a command line argument interface to
 * drawer selection. Throws exception if no drawer found */
DrawerType getDrawerType(const std::string& drawerName);

/* Return drawer name based on type, opposite of above */
std::string getDrawerName(const DrawerType& type);

/* Generic helper function to save a surface to a PNG at "filename"
 * return true on success */
bool saveSurfacePNG(SDL_Surface* surface, const std::string& filename);

}

#endif
