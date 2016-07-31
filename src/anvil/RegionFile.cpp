/*
 ** 2011 January 5
 **
 ** The author disclaims copyright to this source code.  In place of
 ** a legal notice, here is a blessing:
 **
 **    May you do good and not evil.
 **    May you find forgiveness for yourself and forgive others.
 **    May you share freely, never taking more than you give.
 **/
/*
 * 2011 February 16
 *
 * This source code is based on the work of Scaevolus (see notice above).
 * It has been slightly modified by Mojang AB (constants instead of magic
 * numbers, a chunk timestamp header, and auto-formatted according to our
 * formatter template).
 *
 */

// Interfaces with region files on the disk

/*

 Region File Format

 Concept: The minimum unit of storage on hard drives is 4KB. 90% of Minecraft
 chunks are smaller than 4KB. 99% are smaller than 8KB. Write a simple
 container to store chunks in single files in runs of 4KB sectors.

 Each region file represents a 32x32 group of chunks. The conversion from
 chunk number to region number is floor(coord / 32): a chunk at (30, -3)
 would be in region (0, -1), and one at (70, -30) would be at (3, -1).
 Region files are named "r.x.z.data", where x and z are the region coordinates.

 A region file begins with a 4KB header that describes where chunks are stored
 in the file. A 4-byte big-endian integer represents sector offsets and sector
 counts. The chunk offset for a chunk (x, z) begins at byte 4*(x+z*32) in the
 file. The bottom byte of the chunk offset indicates the number of sectors the
 chunk takes up, and the top 3 bytes represent the sector number of the chunk.
 Given a chunk offset o, the chunk data begins at byte 4096*(o/256) and takes up
 at most 4096*(o%256) bytes. A chunk cannot exceed 1MB in size. If a chunk
 offset is 0, the corresponding chunk is not stored in the region file.

 Chunk data begins with a 4-byte big-endian integer representing the chunk data
 length in bytes, not counting the length field. The length must be smaller than
 4096 times the number of sectors. The next byte is a version field, to allow
 backwards-compatible updates to how chunks are encoded.

 A version of 1 represents a gzipped NBT file. The gzipped data is the chunk
 length - 1.

 A version of 2 represents a deflated (zlib compressed) NBT file. The deflated
 data is the chunk length - 1.

 */
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "anvil/RegionFile.h"

/* I/O Helpers, mostly from cNBT
 * ========================================================================= */

/* are we running on a little-endian system? */
static int little_endian()
{
    uint16_t t = 0x0001;
    char c[2];
    memcpy(c, &t, sizeof t);
    return c[0];
}

//Swap bytes from BE <-> LE
static void* swap_bytes(void* s, size_t len)
{
    for(char* b = (char*)s,
            * e = b + len - 1;
        b < e;
        b++, e--)
    {
        char t = *b;

        *b = *e;
        *e = t;
    }

    return s;
}

/* big endian to native endian. works in-place */
static void* be2ne(void* s, size_t len)
{
    return little_endian() ? swap_bytes(s, len) : s;
}

int readInt(std::iostream& s)
{
    int ret = 0;
    s.read((char*)&ret, 4);
    be2ne(&ret, 4);
    return ret;
}

byte readByte(std::iostream& s)
{
    byte ret = 0;
    s.read((char*)&ret, 1);
    return ret;
}

/* RegionFile
 * ========================================================================= */

RegionFile::RegionFile()
    : isLoaded(false)
    , knowAllChunks(false)
{

}

RegionFile::RegionFile(const std::string& path)
    : RegionFile()
{
    load(path);
}

RegionFile::~RegionFile()
{
    for(auto& pair : knownChunkData) {
        nbt_free(pair.second);
    }
}

void RegionFile::load(const std::string& path)
{
    //Load entire file into string
    fileContent = readFile(path);
    if(fileContent.empty()) {
        error("Could not load region file \"", path, "\"");
    }

    //Create stream on file for easy seeking and reading
    file.str(fileContent);

    int fileLength = getLength(file);
    offsets.resize(SECTOR_INTS, 0);

    /* set up the available sector map. Sectors 0 and 1 are
     * the regions's metadata, always taken */
    int nSectors = (int)fileLength / SECTOR_BYTES;
    sectorFree.resize(nSectors, true);
    sectorFree[0] = false; // chunk offset table
    sectorFree[1] = false; // for the last modified info

    //Tell me the byte offset of the chunks (sector 1)
    file.seekg(0);
    for (int i = 0; i < SECTOR_INTS; ++i) {
        unsigned offset = readInt(file);
        offsets[i] = offset;
        if (offset != 0 && (offset >> 8) + (offset & 0xFF) <= sectorFree.size()) {
            for (unsigned sectorNum = 0; sectorNum < (offset & 0xFF); ++sectorNum) {
                sectorFree[(offset >> 8) + sectorNum] = false;
            }
        }
    }

    /* The next SECTOR_INTS ints (sector 2) would be the timestamps--the last saved time
     * of the chunk. We don't care about them here, so they're not loaded */

    //Mark this as being loaded correctly
    isLoaded = true;
}

bool RegionFile::hasChunk(int x, int z)
{
    return getOffset(x, z) != 0;
}

const RegionFile::ChunkMap& RegionFile::getAllChunks()
{
    if(!knowAllChunks)
    {
        for(int x = 0; x != 32; ++x) {
        for(int z = 0; z != 32; ++z) {
            nbt_node* nbt = getChunkNBT(x,z);
            if(nbt) {
                knownChunkData[MC_Point{x,z}] = nbt;
            }
        }
        }
        knowAllChunks = true;
    }
    return knownChunkData;
}

/* Returns the NBT tree for the chunk at X and Z */
nbt_node* RegionFile::getChunkNBT(int x, int z)
{
    if (outOfBounds(x, z)) {
        error("Chunk: ", x, z, " out of bounds");
        return nullptr;
    }
    if(!isLoaded) {
        error("Region file not loaded");
        return nullptr;
    }
    if(!hasChunk(x,z)) {
        return nullptr;
    }
    //Check to see if we've cached it before
    auto it = knownChunkData.find(MC_Point{x,z});
    if(it != knownChunkData.end()) {
        return it->second;
    }

    //Now to actually load the chunk

    //Offset of chunk at x,z in bytes
    int offset = getOffset(x, z);

    /* Calculating 4KB sector number, and number of sectors
     * that chunk occupies */
    unsigned sectorNumber = offset >> 8;
    unsigned numSectors = offset & 0xFF;
    if (sectorNumber + numSectors > sectorFree.size()) {
        log("Chunk: ", x, z, "invalid sector");
        return nullptr;
    }

    //Seek to the chunk sector. Length of chunk is the first int at sector
    file.seekg(sectorNumber * SECTOR_BYTES);
    unsigned length = readInt(file);
    if (length > SECTOR_BYTES * numSectors) {
        error("Chunk: ", x, z, "invalid length: ", length, " > 4096 * ", numSectors);
        return nullptr;
    }

    /* Next byte: Version of compression. Either GZIP (1) or DEFLATE (2),
     * But according to Wiki, only 2 is used in practice. Ignoring this */
    readByte(file);

    //Next data: Compressed chunk NBT data. What we're after!
    std::vector<char> data(length);
    file.read(data.data(), length);
    nbt_node* nbt = nbt_parse_compressed(data.data(), length);

    //Record that we know chunk data at this coordinate before returning
    knownChunkData[MC_Point{x,z}] = nbt;

    //Return NBT data
    return nbt;
}

/* is this an invalid chunk coordinate? */
bool RegionFile::outOfBounds(int x, int z) {
    return x < 0 || x >= 32 || z < 0 || z >= 32;
}

int RegionFile::getOffset(int x, int z) {
    return offsets[x + z * 32];
}
