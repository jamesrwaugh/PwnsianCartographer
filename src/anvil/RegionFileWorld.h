#ifndef REGIONFILEWORLD_H
#define REGIONFILEWORLD_H
#include <map>
#include "anvil/RegionFile.h"

/* RegionFileWorld is a world of Anvil regions (RegionFiles). It traverses
 * the region/ directory and loads each region, and provides
 * a cool interface, of course, namely getAllRegions() and getSize() */

class RegionFileWorld
{
public:
    //- A region coordinate such as -1,2 -- from the .mca filenames
    typedef MC_Point RegionCoord;

    //- Map type for {-1,2} -> region data
    typedef std::map<RegionCoord, RegionFile> RegionMap;

public:
    /* Initialize from the root of a Minecraft world.
     * i.e, where level.dat is located. This will load the rest. */
    RegionFileWorld(std::string rootpath);

    //Return all the regions
    RegionMap& getAllRegions();

    //Get X/Z size of the world in blocks
    MC_Point getSize();

private:
    //From a "r.1.-1.mca", get the 1 and -1. Also validates the name.
    // return.first == true if valid, return.second is the value if valid
    std::pair<bool,RegionCoord> parseFilename(const std::string& filename);

    /* Stored regions. Maps a pair of integers, such as
     * -1,0 to the .mca region. */
    RegionMap regions;
};

#endif
