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
    : heightMap(nullptr)
    , haveHeightMap(false)
{
    this->chunk = chunk;
}

unsigned ChunkInterface::getBlockID(int x, int y, int z)
{
    //Convert absolute Y to section, and load the secion if needed
    int ySection = absoluteYToSection(y);
    loadYSection(ySection);

    /* The actual Y coordinate is relative to the section, not the whole
     * chunk */
    int sectionY = y % 16;

    /* Get the ID from the section above. If the "Add" array is there,
     * it represents an upper 8 bits of the block ID */
    int index = sectionY*16*16 + z*16 + x;

    Section& section = ySectionMap[ySection];
    unsigned id = section.Blocks[index];
    //std::vector<byte> blockCopy(section.Blocks, section.Blocks+4096);
    if(section.Add) {
        byte upperID = section.Add[index];
        id |= (upperID << 8);
    }

    return id;
}

unsigned ChunkInterface::getHighestSolidBlockID(int x, int z)
{
    //We need to get the height map first.
    if(!haveHeightMap) {
        nbt_node* heightNode = nbt_find_by_path(chunk, ".Level.HeightMap");
        if(heightNode != NULL) {
            heightMap = getIntArray(heightNode, "HeightMap");
            haveHeightMap = true;
        }
    }

    //Sanity
    if(heightMap == nullptr) {
        error("Failed to find heightmap in chunk");
    }

    /* Return the block at X Y Z, where the Y is the heightmap-1 of that
     * X and Z. heightMap at that X,Z location will be the lowest location
     * where light is at full strength, which is air.
     * So, subtracting 1 is the block directly under that. (highest solid) */
    int y = heightMap[z*16 + x] - 1;
    if(y < 0) y = 0;

    return getBlockID(x, y, z);
}

void ChunkInterface::loadYSection(int y)
{
    //Range check to keep sanity
    if(y < 0 || y > 15) {
        error("Trying to load Y section ", y, " in chunk");
    }

    //Check if we've already loaded this. If so, stop.
    if(ySectionMap.find(y) != ySectionMap.end()) {
        return;
    }

    //A TAG_Compound of all sections in the chunk...
    nbt_node* sections = nbt_find_by_path(chunk, ".Level.Sections");

    //...Find the section with the Y index we're looking for
    nbt_node* neededSecion = nbt_find(sections, nbtPredicates::findYSection, &y);

    //Friendly check
    if(!sections || !neededSecion) {
        error("Failed to find Y section ", y, " in chunk");
    }

    //We've found it now, so store in the section cache
    if(neededSecion) {
        Section& newSection = ySectionMap[y];
        newSection.Y = y;
        newSection.Blocks     = getByteArray(neededSecion, "Blocks");
        newSection.Add        = getByteArray(neededSecion, "Add");
        newSection.BlockLight = getByteArray(neededSecion, "BlockLight");
    }
}

Uint8 ChunkInterface::getHighestLightLevel(int x, int z)
{
    (void)x;
    (void)z;
    return 0;
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


int ChunkInterface::absoluteYToSection(int y)
{
    return y / 16;
}

