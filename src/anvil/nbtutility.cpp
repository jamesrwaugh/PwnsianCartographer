#include "anvil/nbtutility.h"

namespace nbtutil
{

namespace predicates
{

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

unsigned char* getByteArray(nbt_node* src, const char* name)
{
    nbt_node* node = nbt_find_by_name(src, name);
    if(node && node->type == TAG_BYTE_ARRAY) {
        return node->payload.tag_byte_array.data;
    }
    return nullptr;
}

int* getIntArray(nbt_node* src, const char* name)
{
    nbt_node* node = nbt_find_by_name(src, name);
    if(node && node->type == TAG_INT_ARRAY) {
        return node->payload.tag_int_array.data;
    }
    return nullptr;
}

}

