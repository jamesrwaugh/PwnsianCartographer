#ifndef NBTUTILITY_H
#define NBTUTILITY_H

#include <nbt/nbt.h>

namespace nbtutil {

namespace predicates
{

/* Used to locate a node which has a Tag_Byte("Y") directly below it
 * equal to the one passed in "aux" */
bool findYSection(const nbt_node* node, void* aux);

}

/* NBT helper functions to just get me a pointer to payload data
 * given by a name in a node */
unsigned char* getByteArray(nbt_node* src, const char* name);
int* getIntArray(nbt_node* src, const char* name);

}

#endif // NBTUTILITY_H
