#include <iostream>
#include <map>
#include "utility/utility.h"
#include "anvil/RegionFileWorld.h"
#include "draw/NormalDrawer.h"
#include "docopt-cpp/docopt.h"

static const char USAGE[] =
R"(Pwnsian Cartographer, Minecraft World Renderer

    Usage:
      PwnsianCartographer <world> 
          [--gridlines] [--items-zip <filename>] 
          [--config-file <filename>] 
          [--threads <n>] [--scale <scale>]
      PwnsianCartographer ( -h | --help )

    Options:
      -h --help            Show this screen.
      --gridlines          Add region-sized gridlines to output
      --items-zip <file>   Load block colors from this .zip file [default: items.zip]
      --config-file <file> Use a configuraiton file for options
      --scale <scale>      Scale output. 1x,2x,... [default: 1]
      --threads <num>      Limit number of rendering threads. [default: #CPU Cores]
)";

int main(int argc, char** argv)
{
    try
    {
        auto args = docopt::docopt(USAGE, { argv+1, argv+argc }, true); 

        for(auto const& arg : args) {
            std::cout << arg.first <<  " " << arg.second << std::endl;
        }

        exit(1);
            

        //Load
        std::string worldName(argv[1]);
        RegionFileWorld world(worldName);
        draw::NormalDrawer drawer;

        //Draw
        SDL_Surface* surface = drawer.renderWorld(world);

        //Output
        draw::saveSurfacePNG(surface, removePath(worldName)+"-output"+".png");
    }
    catch(std::exception& ex) {
        log("Something Happened: ", ex.what());
        exit(1);
    }

    return 0;
}
