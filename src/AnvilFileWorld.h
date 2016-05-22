#ifndef ANVILFILEWORLD_H
#define ANVILFILEWORLD_H

#include <map>
#include "AnvilFile.h"

/* RegionFileWorld is a world of regions. it traverses
 * the region/ directory and loads each region, and provides
 * a cool interface, of course */

class RegionFileWorld
{
public:
    //- A region coordinate such as -1,2 from the .mca filenames
    //- Map type for {-1,2} -> RegionData
    typedef std::pair<int,int> RegionCoord;
    typedef std::map<RegionCoord, RegionFile> RegionMap;

    /* Initialize from the root of a Minecraft world.
     * i.e, where level.dat is located. This will load the rest. */
    RegionFileWorld(std::string rootpath);

    //Return all the regions
    RegionMap& regionMap();

private:
    //From a r.1.-1.mca, get the 1 and -1. Also validates the name.
    // return.first == true if valid, return.second is the value if value
    std::pair<bool,RegionCoord> parseFilename(const std::string& filename);

    /* Stored regions. Maps a pair of integers, such as
     * -1,0 to the .mca region. */
    RegionMap regions;
};

#endif // ANVILFILEWORLD_H
