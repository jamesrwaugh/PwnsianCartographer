#ifndef REGIONFILE_H
#define REGIONFILE_H
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>

#include <nbt/nbt.h>
#include "types.h"

/* Interface to a Anvil .mca Region file.
 * Converted from Java from http://pastebin.com/niWTqLvk
 * Stripped + optimized to only include reading */

class RegionFile
{
public:
    //Map of chunk coordinates to their cached NBT data
    typedef std::map<MC_Point, nbt_node*> ChunkMap;

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
    //"offsets" Indicates the offset in bytes into the region file of each chunk
    //"sectorFree" Indicates if a sector is free or not.
    std::vector<int> offsets;
    std::vector<bool> sectorFree;
    bool isLoaded;
    bool knowAllChunks;

    //The input steam to the file's content, and the file's content in full
    //knownChunkData is the most important, all the stored chunks data.
    std::stringstream file;
    std::string fileContent;
    ChunkMap knownChunkData;

    //is this an invalid chunk coordinate?
    bool outOfBounds(int x, int z);

    //Return the byte offset of a chunk at X and Z
    int getOffset(int x, int z);
};

#endif // ANVILFILE_H
