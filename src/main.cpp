#include <iostream>
#include <nbt/nbt.h>
#include <lodepng.h>
#include <json11.hpp>

#include "blocks.h"
#include "AnvilFile.h"
#include "AnvilFileWorld.h"
#include "ChunkInterface.h"
#include "draw.h"
#include "utility.h"

int main(int argc, char** argv)
{
    if(argc < 2) {
        log("Call with a Minecraft world folder");
        exit(1);
    }

    try
    {
        //Load
        RegionFileWorld world(argv[1]);
        draw::Drawer drawer;

        //Draw
        SDL_Surface* surface = drawer.renderWorld(world);

        //Output
        draw::saveSurfacePNG(surface, std::string(argv[1])+"-output"+".png");
    }
    catch(std::exception& ex) {
        log("Something Happened: ", ex.what());
        exit(1);
    }

    return 0;
}
