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
        log("Call with a .mca file");
        exit(1);
    }

    try
    {
    #if 0
        RegionFile file(argv[1]);
        #if 1
        SDL_Surface* render = draw::renderRegion(file);
        lodepng::encode("output-region.png", (unsigned char*)render->pixels, 32*16, 32*16);

        /*
        blocks::BlockColors colors;
        colors.load("items.zip", "items_color_cache.json");
        colors.getBlockColor(429);*/
        #endif
    #else
        RegionFileWorld file(argv[1]);
        MC_Point size = file.getSize();
        log("Size:", size.x, " " , size.z);
    #if 1
        draw::Drawer drawer;
        SDL_Surface* surface = drawer.renderWorld(file);
        draw::saveSurfacePNG(surface, "output-world.png");
    #endif
        #if 0
        for(auto& pair : file.getAllRegions())
        {
            SDL_Surface* render = drawer.renderRegion(pair.second);

            //Write the result
            std::stringstream ss;
            ss << "output-" << pair.first.first <<  "-" << pair.first.second << ".png";
            std::cout << ss.str() << std::endl;
            lodepng::encode(ss.str(), (unsigned char*)render->pixels, 32*16, 32*16);

            SDL_FreeSurface(render);
        }
        #else

        #endif
    #endif
    }
    catch(std::exception& ex) {
        log("Something Happened: ", ex.what());
        exit(1);
    }

    return 0;
}
