#ifndef TYPES_H
#define TYPES_H
#include <stdint.h>

//Making byte a type
typedef uint8_t byte;

//A 2D point in the Minecraft world.
struct MC_Point
{
    int x, z;

    MC_Point(int nx, int nz) : x(nx), z(nz) { }

    bool operator<(const MC_Point& other) const {
        return x < other.x || (!(other.x < x) && z < other.z);
    }
};

#endif // TYPES_H
