#ifndef CHUNKINTERFACE_H
#define CHUNKINTERFACE_H

#include <array>
#include <map>

#include <nbt/nbt.h>
#include "types.h"
#include "blocks.h"

class ChunkInterface
{
public:
    ChunkInterface(nbt_node* chunk);

    /* Generic return a block ID at X,Y,Z */
    unsigned getBlockID(int x, int y, int z);

    /* Important! Gives the ID of the highest block at X,Z.
     * For a top-down view, this is the block we render. */
    unsigned getHighestSolidBlockID(int x, int z);

    /* Important! Gives the light level (0-15) of the highest block at X,Z.
     * With above, this can be used to draw lights */
    Uint8 getHighestLightLevel(int x, int z);

private:
    /* A "Section" is a 16x16x16 cube of blocks in a chunk. There are up
     * to 16 sections, with a "Y" from 0 to 15, with 0 being bedrock layer.
     * Some sections are not stored, to save space.
     * This is a small interface that points directly into a nbt_node
     * of a chunk Section (i.e, it doesn't own the pointers) */
    struct Section
    {
        //TAG_Byte_Array("Blocks") 16x16x16
        byte* Blocks = nullptr;

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

private:
    /* NBT helper functions to just get me a pointer to payload data
     * given by a name in a node */
    unsigned char* getByteArray(nbt_node* src, const char* name);
    int* getIntArray(nbt_node* src, const char* name);
};

#endif // CHUNKINTERFACE_H
