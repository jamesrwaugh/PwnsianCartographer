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
        SDL_Point size = file.getSize();
        log("Size:", size.x, " ", size.y);
        #if 1
        for(auto& pair : file.regionMap())
        {
            SDL_Surface* render = draw::renderRegion(pair.second);

            //Write the result
            std::stringstream ss;
            ss << "output-" << pair.first.first <<  "-" << pair.first.second << ".png";
            std::cout << ss.str() << std::endl;
            lodepng::encode(ss.str(), (unsigned char*)render->pixels, 32*16, 32*16);

            SDL_FreeSurface(render);
        }
        #else
        for(auto& pair : file.regionMap())
        {
            RegionFile& region = pair.second;
            for(const RegionFile::ChunkMap::value_type& pair : region.getAllChunks())
            {
                //Render a chunk to a lodepng-readable array
                SDL_Surface* render = draw::renderChunk(pair.second);

                //Write the result
                std::stringstream ss;
                ss << "output-" << pair.first.first <<  "-" << pair.first.second << ".png";
                lodepng::encode(ss.str(), (unsigned char*)render->pixels, 16, 16);

                SDL_FreeSurface(render);
            }
        }
        #endif
    #endif
    }
    catch(std::exception& ex) {
        log("Problem: ", ex.what());
        exit(1);
    }

    return 0;
}
