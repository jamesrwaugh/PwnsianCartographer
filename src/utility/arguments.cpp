#include <iostream>
#include <SDL2/SDL.h>
#include "draw/draw.h"
#include "config.h"
#include "utility/utility.h"
#include "utility/arguments.h"

namespace arguments
{

Args::Args(const std::string& USAGE, int argc, char** argv)
{
    auto args = docopt::docopt(USAGE, { argv+1, argv+argc }, true, __DATE__);

#if 0
    for(auto const& arg : args) {
        std::cout << arg.first <<  " " << arg.second << std::endl;
    }
#endif

    //<world> is the only required command line arugment
    worldName = args["<world>"].asString();

    //Prase. If the config file is specified, load from that instead
    auto& configOption = args["--config-file"];
    if(configOption) {
         fromConfigFile(configOption.asString());
    } else {
        fromDocOpt(args);
    }

    //Ensure parsed arguments are in valid ranges
    validateArguments();
}

void Args::fromDocOpt(std::map<std::string, docopt::value>& args)
{
    numThreads = args["--threads"].asLong();
    gridlines = args["--gridlines"].asBool();
    scale = args["--scale"].asLong();
    itemZipFilename = args["--items-zip"].asString();

    //User choses drawer type; default normal
    auto& renderTypeArg = args["<render-type>"];
    if(renderTypeArg) { 
        renderTypeStr = renderTypeArg.asString();
    } else {
        renderTypeStr = draw::getDrawerName(draw::DrawerType::Normal);
    }
    requestedDrawer = draw::getDrawerType(renderTypeStr); //Also validates type here

    //output has no default, so we need to check for null.
    //validateArguments() will handle empty output string anyway
    auto& outputArg = args["--output"];
    if(outputArg) {
        outputFilename = outputArg.asString();
    }
}

void Args::fromConfigFile(const std::string& configFilename)
{
    config::setFilename(configFilename);
    auto& config = config::get();

    numThreads = config.GetInt("threads");
    gridlines = config.GetInt("gridlines");
    scale = config.GetInt("scale");
    itemZipFilename = config.GetString("items-zip");
    outputFilename = config.GetString("output");
    std::string renderType = config.GetString("render-type");
    requestedDrawer = draw::getDrawerType(renderType); //Also validates type here
}

void Args::validateArguments()
{
    if(numThreads <= 0) { //This is actually the default case
        numThreads = SDL_GetCPUCount();
    }
    if(scale < 1) {
        scale = 1;
    }
    if(outputFilename.empty()) {
        outputFilename = removePath(worldName)+"-output-"+renderTypeStr+".png";
    }
    if(!fileExists(itemZipFilename)) {
        error("Could not find items archive \"", itemZipFilename, "\"");
    }
}

}
