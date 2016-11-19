#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include "utility/keyvalues.h"

/* Singleton wrapper around a KeyValue object, used
 * to load from the config file */

class config
{
public:
    // Return the KeyValue instance
    static KeyValue& get();

    //Load from name instead of "config.ini"
    static void setFilename(const std::string& name);

private:
    static bool isLoaded;
    static std::string filename;
    static KeyValue instance;
};

#endif
