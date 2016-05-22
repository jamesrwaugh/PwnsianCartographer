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
        #if 0
        RegionFile file(argv[1]);
        RegionFile::ChunkMap chunks = file.getAllChunks();
        std::cout << chunks.size() << " Chunks..." << std::endl;
        for(const auto& pair : chunks)
        {
            ChunkInterface iface(pair.second);
            unsigned id = iface.getBlockID(0,0,0);

            std::cout << "========================================" << std::endl;
            std::cout << pair.first.first << " " << pair.first.second << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << nbt_dump_ascii(pair.second) << std::endl;
            std::cout << "========================================" << std::endl;
        }
        #else
        blocks::BlockColors colors;
        colors.load("items.zip", "items_color_cache.json");
        colors.getBlockColor(429);
        #endif
    #else
        RegionFileWorld file(argv[1]);
        SDL_Point size = file.getSize();
        log("Size:", size.x, " ", size.y);
        #if 0
        for(auto& pair : file.regionMap())
        {
            RegionFile& region = pair.second;
            if(region.hasChunk(0,0)) {
                char* ascii = nbt_dump_ascii(region.getChunkNBT(0,0));
                std::cout << pair.first.first << " " << pair.first.second << " -> " << ascii << std::endl;
            }
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
