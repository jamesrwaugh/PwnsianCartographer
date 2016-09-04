#include <iostream>
#include "draw/draw.h"
#include "utility/arguments.h"
#include "utility/utility.h"

static const char USAGE[] =
R"(Pwnsian Cartographer, Minecraft World Renderer

Usage:
    PwnsianCartographer <world> <render-type>
        [-g | --gridlines]
        [-i --items-zip=<filename>]
        [-t --threads=<n>]
        [-s --scale=<amount>]
        [-o --output=<file>]
    PwnsianCartographer <world> (--config-file=<file>)
    PwnsianCartographer ( -h | --help )

Options:
    render-type             Output render type. Valid: (normal, height)
    -h --help               Show this screen.
    -g --gridlines          Add region-sized gridlines to output
    -c --config-file <file> Use a configuraiton file for all options
    -i --items-zip <file>   Load block colors from this .zip file [default: items.zip]
    -s --scale <amount>     Scale output. 1x, 2x, ... [default: 1]
    -t --threads <n>        Limit number of rendering threads; 0 for #CPU Cores [default: 0]
    -o --output <file>      Place output image in file instead of in "."
)";

int main(int argc, char** argv)
{
    try
    {
        arguments::Args args(USAGE, argc, argv);

        auto drawer = draw::createDrawer(args.requestedDrawer);
        SDL_Surface* render = drawer->renderWorld(args.worldName, args);

        draw::saveSurfacePNG(render, args.outputFilename);
    }
    catch(std::exception& ex) {
        log("Something Happened: ", ex.what());
        exit(1);
    }

    return 0;
}
