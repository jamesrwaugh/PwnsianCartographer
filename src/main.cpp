#include <iostream>
#include <nbt/nbt.h>

#include "utility.h"
#include "blocks/blocks.h"
#include "anvil/RegionFile.h"
#include "anvil/RegionFileWorld.h"
#include "anvil/ChunkInterface.h"
#include "draw/NormalDrawer.h"

//Return part of a path after the last slash, but before any trailing slashes
std::string removePath(const std::string& path)
{
    //Absolute last alnum character in string,
    //and last slash in string, before last alnum chracter
    size_t lastAlphaPos = path.find_last_not_of("\\/");
    size_t firstAlphaPos = path.find_last_of("\\/", lastAlphaPos);

    return path.substr(firstAlphaPos + 1, lastAlphaPos - firstAlphaPos);
}

int main(int argc, char** argv)
{
    if(argc < 2) {
        log("Call with a Minecraft world folder");
        exit(1);
    }

    try
    {
        //Load
        std::string worldName(argv[1]);
        RegionFileWorld world(worldName);
        draw::NormalDrawer drawer;

        //Draw
        SDL_Surface* surface = drawer.renderWorld(world);

        //Output
        draw::saveSurfacePNG(surface, removePath(worldName)+"-output"+".png");
    }
    catch(std::exception& ex) {
        log("Something Happened: ", ex.what());
        exit(1);
    }

    return 0;
}
