#include <fstream>
#include "utility/utility.h"
#include "config.h"

bool config::isLoaded = false;
KeyValue config::instance;
std::string config::filename;

KeyValue& config::get()
{
    if(!isLoaded) {
        std::ifstream file(filename);
        if(file.is_open()) {
            instance.LoadFromFile(filename);
            isLoaded = true;
        } else {
            error("Could not load config file \"", filename, "\"");
        }
    }
    return instance;
}

void config::setFilename(const std::string& name)
{
    filename = name;
}
