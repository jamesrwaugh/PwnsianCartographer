#include <iostream>
#include "draw/NormalDrawer.h"
#include "utility/arguments.h"
#include "utility/utility.h"

static const char USAGE[] =
R"(Pwnsian Cartographer, Minecraft World Renderer

Usage:
    PwnsianCartographer <world> 
        [-g | --gridlines] 
        [-i --items-zip=<filename>] 
        [-c --config-file=<filename>] 
        [-t --threads=<n>] 
        [-s --scale=<amount>]
        [-o --output=<file>]
    PwnsianCartographer ( -h | --help )

Options:
    -h --help               Show this screen.
    -g --gridlines          Add region-sized gridlines to output
    -i --items-zip <file>   Load block colors from this .zip file [default: items.zip]
    -c --config-file <file> Use a configuraiton file for options
    -s --scale <amount>     Scale output. 1x, 2x, ... [default: 1]
    -t --threads <n>        Limit number of rendering threads; 0 for #CPU Cores [default: 0]
    -o --output <file>      Place output image in file instead of in "."
)";

int main(int argc, char** argv)
{
    try
    {
        arguments::Args args(USAGE, argc, argv);
       
        //Draw
        draw::NormalDrawer drawer;
        SDL_Surface* render = drawer.renderWorld(args.worldName, args);

        //Output
        draw::saveSurfacePNG(render, args.outputFilename);
    }
    catch(std::exception& ex) {
        log("Something Happened: ", ex.what());
        exit(1);
    }

    return 0;
}
