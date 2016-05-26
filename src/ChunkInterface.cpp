#include "utility.h"
#include "ChunkInterface.h"

/* ============================================================================
 * Predicates and aux functions used with nbt_find, nbt_map */

namespace nbtPredicates
{

/* Used to locate a node which has a Tag_Byte("Y") directly below it
 * equal to the one passed in "aux" */

bool findYSection(const nbt_node* node, void* aux)
{
    nbt_node* y = nbt_find_by_name((nbt_node*)node, "Y");
    if(y) {
        int neededY = *(int*)aux;
        return y->payload.tag_byte == neededY;
    }
    return false;
}

}

/* ============================================================================
 * Main ChunkInterface */

ChunkInterface::ChunkInterface(nbt_node* chunk)
{
    this->chunk = chunk;

    //Attempt to load the heightmap of the chunk on construction
    //This is important for finding the highest blocks
    loadHeightMap();
}

unsigned ChunkInterface::getBlockID(int x, int y, int z)
{
    /* Convert absolute Y to section, and load the section if needed
     * if we don't have its section, return invalid block */
    int ySection = absoluteYToSection(y);
    loadYSection(ySection);
    if(sections[ySection].state == Section::NOTFOUND) {
        return blocks::invalidID;
    }

    /* The actual Y coordinate is relative to the section, not the whole
     * chunk */
    int sectionY = y % 16;

    /* Get the ID from the section. If the "Add" array is there,
     * it represents an upper 8 bits of the block ID */
    int index = sectionY*16*16 + z*16 + x;

    Section& section = sections[ySection];
    unsigned id = section.Blocks[index];
    if(section.Add) {
        byte upperID = section.Add[index];
        id |= (upperID << 8);
    }

    return id;
}

unsigned ChunkInterface::getHighestSolidBlockID(int x, int z)
{
    /* Return the block at X Y Z, where the Y is the heightmap of that
     * X and Z. heightMap at that X,Z location will be the lowest location
     * where light is at full strength, which may or may not be air. */
    int y = heightMap[z*16 + x];

    /* Try to get block at the heightmap. Often, this section is not loaded.
     * If it's also not air, like redstone, return it */
    unsigned id = getBlockID(x, y, z);
    if( id != blocks::invalidID && id != 0 ) {
        return id;
    }

    /* Otherwise the highest *solid* block is one-below the heightmap, because
     * the section either isnt' loaded, it it's air */
    return getBlockID(x, std::max(y-1,0), z);
}

void ChunkInterface::loadYSection(int y)
{
    //Range check to keep sanity
    if(y < 0 || y > 15) {
        error("Trying to load Y section ", y, " in chunk");
    }

    //Check if we've already loaded this. If so, stop.
    if(sections[y].state != Section::UNDISCOVERED) {
        return;
    }

    //A TAG_Compound of all sections in the chunk...
    nbt_node* nbtsections = nbt_find_by_path(chunk, ".Level.Sections");

    //...Find the section with the Y index we're looking for
    nbt_node* neededSecion = nbt_find(nbtsections, nbtPredicates::findYSection, &y);

    //Friendly validity check
    if(!nbtsections || !neededSecion) {
        sections[y].state = Section::NOTFOUND;
        return;
    }

    //We've found it now, so store in the section cache
    if(neededSecion)
    {
        Section& sect = sections[y];
        sect.state = Section::FOUND;
        sect.Y = y;
        sect.Blocks     = getByteArray(neededSecion, "Blocks");
        sect.Add        = getByteArray(neededSecion, "Add");
        sect.BlockLight = getByteArray(neededSecion, "BlockLight");

        if(!sect.Blocks || !sect.BlockLight) {
            error("Failed to find Blocks array in chunk section ", y);
        }
    }
}

void ChunkInterface::loadHeightMap()
{
    //The height map is a int array at the following path in the chunk.
    nbt_node* heightNode = nbt_find_by_path(chunk, ".Level.HeightMap");

    heightMap = getIntArray(heightNode, "HeightMap");
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

unsigned char* ChunkInterface::getByteArray(nbt_node* src, const char* name)
{
    nbt_node* node = nbt_find_by_name(src, name);
    if(node && node->type == TAG_BYTE_ARRAY) {
        return node->payload.tag_byte_array.data;
    }
    return nullptr;
}

int* ChunkInterface::getIntArray(nbt_node* src, const char* name)
{
    nbt_node* node = nbt_find_by_name(src, name);
    if(node && node->type == TAG_INT_ARRAY) {
        return node->payload.tag_int_array.data;
    }
    return nullptr;
}
