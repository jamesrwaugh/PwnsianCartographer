#ifndef BLOCKS_H
#define BLOCKS_H

#include <SDL2/SDL.h>
#include <string>
#include <map>
#include "ZipLib/ZipArchiveEntry.h"

/* Routines to handle returning RGB values for
 * block IDs, and misc utility color functions.
 * This loads png images from a .zip to get average colors */

namespace blocks
{

//How do we identify a block?
//CAREFUL: If something like "404.png" is passed in, this breaks FIXME
//=============================================================================
struct BlockID
{
public:
    unsigned id = 0;    // The block ID
    unsigned meta = 0;  // metadata, aka damage

public:
    BlockID(unsigned id, unsigned meta);

    /* Construct from string in the form of "301-4"
     * i.e, "id-meta". See .cpp */
    BlockID(const std::string& string);

    /* For std::map. Compares on ID first, and if they're
     * equal, a tie is decided by the meta. */
    bool operator<(const BlockID& other) const;
    bool operator==(const BlockID& other) const;
    bool operator!=(const BlockID& other) const;

    //Conver to string, id-meta, same as parsed
    operator std::string() const;
};

//Represents a invalid block id
static const BlockID invalidID = {UINT_MAX,UINT_MAX};

/* Class to load and manage a .zip file of .pngs of block IDS, and extract
 * a color for each, for drawing. Caches colors in a .json file, specified by
 * cacheFileName in the .load, to avoid recomputing them each time
 *
 * The expected .zip is from here: http://minecraft-ids.grahamedgecombe.com/api */
//=============================================================================
class BlockColors
{
public:
    BlockColors();
   ~BlockColors();
    BlockColors(const BlockColors&) = delete;

    /* Open the block .png zip from a file
     * Basically, the one from http://minecraft-ids.grahamedgecombe.com/api
     * "cacheFileName" is a json file that stores cached colors, is only
     * recomputed if an ID in the .zip changes */
    void load(const std::string& zipFileName,
              const std::string& cacheFileName = "blockcolors.json");

    /* Return a color for a block. This is based off of the
     * png for its ID */
    SDL_Color getBlockColor(unsigned id, unsigned meta = 0) const;
    SDL_Color getBlockColor(const BlockID& blockid) const;

    /* Return the pixel format used in color operations.
     * Can be useful outside BlockColors */
    const SDL_PixelFormat* pixelFormat() const;

    /* If we have valid .zip data or not */
    bool isLoaded() const;

private:
    //Filenames
    std::string zipFileName, cacheFileName;

    //See isLoaded()
    bool loaded;

    //SDL Pixel format for converting pixels to RGBA values and back;
    //is always SDL_PIXELFORMAT_RGBA8888
    SDL_PixelFormat* rgba;

    //Read a ZipArchiveEntry::Ptr into bytes
    std::vector<char> readZipEntry(const ZipArchiveEntry::Ptr& blockImage);

    //Give us a single color for a block from the .zip
    SDL_Color computeColor(const ZipArchiveEntry::Ptr& blockImage);

    //Writes out a new JSON cache, in case of computeColor being used
    void saveNewJsonCache() const;

    //Map of a blockID -> {color, .zip CRC32}
    //The CRC is the hash of the png used to generated the color.
    std::map<BlockID, std::pair<SDL_Color, unsigned>> blockColors;
};

}

#endif // BLOCKS_H
