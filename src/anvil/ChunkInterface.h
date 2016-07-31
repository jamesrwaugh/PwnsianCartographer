#ifndef CHUNKINTERFACE_H
#define CHUNKINTERFACE_H
#include <array>
#include <nbt/nbt.h>

#include "types.h"
#include "blocks/blocks.h"

/* ChunkInterface
 * ChunkInterface is a wrappper around a raw nbt_node* pointing to
 * a chunk within a region. It provides usable functions to easily
 * get block data */

class ChunkInterface
{
public:
    ChunkInterface(nbt_node* chunk);

    /* Generic return a block ID at X,Y,Z */
    blocks::BlockID getBlockID(int x, int y, int z);

    /* Important! Gives the ID of the highest block at X,Z.
     * For a top-down view, this is the block we render. */
    blocks::BlockID getHighestSolidBlockID(int x, int z);

    /* Important! Gives the light level (0-15) of the highest block at X,Z.
     * With above, this can be used to draw lights */
    Uint8 getHighestLightLevel(int x, int z);

private:
    /* A "Section" is a 16x16x16 cube of blocks in a chunk. There are up
     * to 16 sections, with a "Y" from 0 to 15, with 0 being bedrock layer.
     * Some sections are not stored, to save space.
     * This is a small interface that points directly into a nbt_node
     * of a chunk Section (i.e, it doesn't own the pointers) */
    class Section
    {
    public:
        void load(nbt_node* chunk, int y);
        bool isUndiscovered() const;
        bool isInvalid() const;
        bool isValid() const;
        blocks::BlockID getBlockID(int x, int y, int z);

    private:
        //TAG_Byte_Array("Blocks") 16x16x16
        byte* Blocks = nullptr;

        //TAG_Byte_Array("Data") 16x16x16
        byte* Data = nullptr;

        //TAG_Byte_Array("Add") 16x16x16
        byte* Add = nullptr;

        //TAG_Byte_Array("SkyLight") 16x16 FIXME
        byte* BlockLight = nullptr;

        //TAG_Byte("Y")
        byte Y = 255;

        //State of the section in this chunk.
        enum {
            UNDISCOVERED, //We haven't looked for this section before
            NOTFOUND,     //We've looked, but it's not here
            FOUND         //We've looked, and it's a valid secion
        } state = UNDISCOVERED;
    };

private:
    //The raw chunk NBT data (How to manage this pointer??)
    nbt_node* chunk;

    //In a chunk, there are up to 16 "Y" sections, which are 16 block high.
    //Given a generic Y 0-256, what section should it be in? (just /= 16)
    int absoluteYToSection(int y);

    //Tag_Int_Array("HeightMap") | 16x16
    //Indiciates the highest block at an X/Z position in the chunk
    int* heightMap;
    void loadHeightMap();

    /* Array of known Y sections to their outer section data
     * "loadYSection" looks up a Y section in "chunk" and adds
     * it to sections for use */
    std::array<Section,16> sections;
    void loadYSection(int y);
};

#endif // CHUNKINTERFACE_H
