#include "utility/utility.h"
#include "anvil/nbtutility.h"
#include "anvil/ChunkInterface.h"

/* ============================================================================
 * Section */

void ChunkInterface::Section::load(nbt_node* chunk, int y)
{
    //A TAG_Compound of all sections in the chunk...
    nbt_node* nbtsections = nbt_find_by_path(chunk, ".Level.Sections");

    //...Find the section with the Y index we're looking for
    nbt_node* neededSecion = nbt_find(nbtsections, nbtutil::predicates::findYSection, &y);

    //Friendly validity check; Expected, as not all sections are loaded
    if(!nbtsections || !neededSecion) {
        state = NOTFOUND;
        return;
    }

    state      = FOUND;
    Y          = y;
    Blocks     = nbtutil::getByteArray(neededSecion, "Blocks");
    Add        = nbtutil::getByteArray(neededSecion, "Add");
    BlockLight = nbtutil::getByteArray(neededSecion, "BlockLight");
    Data       = nbtutil::getByteArray(neededSecion, "Data");

    if(!Blocks || !Data || !BlockLight) {
        error("Failed to find data arrays in chunk section ", y);
    }
}

bool ChunkInterface::Section::isUndiscovered() const
{
     return state == UNDISCOVERED;
}

bool ChunkInterface::Section::isInvalid() const
{
    return state == NOTFOUND;
}

bool ChunkInterface::Section::isValid() const
{
    return state == FOUND;
}

blocks::BlockID ChunkInterface::Section::getBlockID(int x, int y, int z)
{
    if(y < 0 || y > 15) {
        error("Trying to read block with y '", y, "'' in chunk ", Y);
    }
    if(!isValid()) {
        return blocks::invalidID;
    }

    /* Get the block ID from the block array. If the "Add" array is there,
     * it represents an upper 8 bits of the block ID */
    int index = y*16*16 + z*16 + x;
    unsigned id = Blocks[index];
    if(Add) {
        id |= (Add[index] << 8);
    }

    /* Metadata is actaully 2048 bytes, 4 bits block.
     * One byte has a block in each nibble */
    unsigned halfindex = index / 2;
    unsigned meta = 0;
    if(index % 2 == 0) {
        meta = Data[halfindex] & 0x0F;
    } else {
        meta = (Data[halfindex] & 0xF0) >> 4;
    }

    return blocks::BlockID(id, meta);
}

/* ============================================================================
 * Main ChunkInterface */

ChunkInterface::ChunkInterface(nbt_node* chunk)
{
    this->chunk = chunk;

    //Attempt to load the heightmap of the chunk on construction.
    //This is important for finding the highest blocks
    loadHeightMap();
}

Uint8 ChunkInterface::getHighestSolidBlockY(int x, int z)
{
    /* To get the highest Y, check the heightmap of that X and Z.
     * The heightMap at that X,Z location will be the lowest location where light
     * is at full strength.
     *
     * PROBLEM: The heightmap method breaks down on columns where a block is at the
     * highest possible Y level, becaue light is not at full at any point in the chunk.
     * In this case, it will be zero.
     */
    int y = heightMap[z*16 + x];

    /* To validate, try to get block at the heightmap (Often, this section is not present).
     * It must also be not air to be the highest *solid* id */
    blocks::BlockID id = getBlockID(x, y, z);
    if( id != blocks::invalidID && id.id != 0 ) {
        return y;
    }

    /* Otherwise the highest *solid* block is one-below the heightmap, because
     * the section either isn't loaded, or it's air */
    return std::max(y-1, 0);
}

blocks::BlockID ChunkInterface::getBlockID(int x, int y, int z)
{
    try {
        //Ensure chunk for this y is loaded
        int section = absoluteYToSection(y);
        loadYSection(section);

        //Y in a section is relative to that 16-high chunk section
        int yInSection = y % 16;
        return sections[section].getBlockID(x, yInSection, z);
    }
    catch(...) {
        return blocks::invalidID;
    }
}

blocks::BlockID ChunkInterface::getHighestSolidBlockID(int x, int z)
{
    return getBlockID(x, getHighestSolidBlockY(x,z), z);
}

void ChunkInterface::loadYSection(int y)
{
    if(y < 0 || y > 15) {
        error("Trying to load invalid Y section ", y, " in chunk");
    }
    if(!sections[y].isUndiscovered()) {
        return;
    }

    sections[y].load(chunk, y);

    if(!sections[y].isValid()) {
        error("Chunk does not have ", y, " section");
    }
}

void ChunkInterface::loadHeightMap()
{
    //The height map is an int array at the following path in the chunk root
    nbt_node* heightNode = nbt_find_by_path(chunk, ".Level.HeightMap");

    heightMap = nbtutil::getIntArray(heightNode, "HeightMap");
    if(heightMap == nullptr) {
        error("Failed to find heightmap in chunk");
    }
}

Uint8 ChunkInterface::getHighestLightLevel(int x, int z)
{
    (void)x;
    (void)z;
    return 0;
}

int ChunkInterface::absoluteYToSection(int y)
{
    return y / 16;
}


