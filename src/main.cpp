#include <iostream>
#include <nbt/nbt.h>
#include <lodepng.h>
#include <json11.hpp>

#include "blocks.h"
#include "AnvilFile.h"
#include "AnvilFileWorld.h"
#include "ChunkInterface.h"
#include "utility.h"

int main(int argc, char** argv)
{
    if(argc < 2) {
        log("Call with a .mca file");
        exit(1);
    }

    try
    {
    #if 1
        #if 1
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
        for(auto& pair : file.regionMap())
        {
            RegionFile& region = pair.second;
            if(region.hasChunk(0,0)) {
                char* ascii = nbt_dump_ascii(region.getChunkNBT(0,0));
                std::cout << pair.first.first << " " << pair.first.second << " -> " << ascii << std::endl;
            }
        }
    #endif
    }
    catch(std::exception& ex) {
        log("Problem: ", ex.what());
        exit(1);
    }

    return 0;
}
