#ifdef __WIN32__
 #include "win32_dirent.h"
#else
 #include <dirent.h>
#endif

#include "utility.h"
#include "AnvilFileWorld.h"

RegionFileWorld::RegionFileWorld(std::string rootpath)
{
    DIR* dp;
    struct dirent* entry;

    //The path we're actually looking for the the region subdir
    rootpath += "/region/";
    dp = opendir(rootpath.c_str());

    /* Traverse the region/ directory, load each .mca, and load it
     * into the region map */
    if(dp != NULL)
    {
        while((entry = readdir(dp)) != NULL)
        {
            auto pair = parseFilename(entry->d_name);
            bool isValid = pair.first;
            RegionCoord coords = pair.second;
            if(isValid) {
                regions[coords].load(rootpath + std::string(entry->d_name));
            }
        }
    }
    else {
        error("Could not load region folder in ", rootpath);
    }
}

RegionFileWorld::RegionMap& RegionFileWorld::regionMap()
{
    return regions;
}

std::pair<bool,RegionFileWorld::RegionCoord>
    RegionFileWorld::parseFilename(const std::string& filename)
{
    /* Sample: "r.1.0.mca" Should have three dots.
     * Then, the substrings between them are examined for the x,z ints */
    int x = 0, z = 0;
    bool valid = false;

    size_t dotpos0 = filename.find('.'),
           dotpos1 = filename.find('.', dotpos0+1),
           dotpos2 = filename.find('.', dotpos1+1);

    if(dotpos0 != std::string::npos &&
       dotpos1 != std::string::npos &&
       dotpos2 != std::string::npos )
    {
        x = std::atoi(filename.substr(dotpos0+1, dotpos1).c_str());
        z = std::atoi(filename.substr(dotpos1+1, dotpos2).c_str());
        valid = true;
    }

    return {valid, RegionCoord { x, z } };
}

