#ifndef ANVILFILE_H
#define ANVILFILE_H

#include <string>
#include <vector>
#include <fstream>
#include <map>

#include <nbt/nbt.h>
#include "types.h"

/* Interface to a .mca Region file.
 * Converted from Java from http://pastebin.com/niWTqLvk */

class RegionFile
{
public:
    //X/Z pairs representing a chunk position
    typedef MC_Point ChunkCoord;

    //Map of chunk coordinates to their cached NBT data
    typedef std::map<ChunkCoord, nbt_node*> ChunkMap;

public:
    //Default constuctor to do nothing, or load on construction.
    //Must call .load() later if default is used.
    RegionFile();
    RegionFile(const std::string& path);
   ~RegionFile();

    //Load the region from a file
    void load(const std::string& path);

    //Is there a chunk at this X and Z?
    bool hasChunk(int x, int z);

    //Return all chunk NBT in the region, mapped by their X/Z coordinate
    const ChunkMap& getAllChunks();

    /* Returns the NBT tree for the chunk at X and Z
     * or nullptr if none exists */
    nbt_node* getChunkNBT(int x, int z);

private:

    //Constants
    //Bytes in a file sector
    static const int SECTOR_BYTES = 4096;
    //Number of ints in a sector
    static const int SECTOR_INTS = SECTOR_BYTES / 4;

    //Variabes
    std::string fileName;
    std::fstream file;
    std::vector<int> offsets;
    std::vector<int> chunkTimestamps;
    std::vector<bool> sectorFree;
    ChunkMap knownChunkData;
    bool isLoaded;
    bool knowAllChunks;

    //is this an invalid chunk coordinate?
    bool outOfBounds(int x, int z);

    //Return the byte offset of a chunk at X and Z
    int getOffset(int x, int z);
};

#endif // ANVILFILE_H
