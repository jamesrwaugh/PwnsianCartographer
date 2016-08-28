#ifndef ARGUMENTS_H
#define ARGUMENTS_H
#include "docopt-cpp/docopt.h"

/* Wrapper to read arguments from either the command line
 * via docopt, or from config file if specified on command line.
 * The output */ 

namespace arguments
{

/* Object holding all PwnsianCartographer command line
 * arguments. Provides interface over config file and docopt,
 *  and validates each */ 

class Args
{
public:
    Args(const std::string& USAGE, int argc, char** argv);

    bool gridlines = false;
    unsigned numThreads = 0;
    unsigned scale = 1;
    std::string worldName; 
    std::string itemZipFilename;
    std::string outputFilename;
private:
    typedef std::map<std::string, docopt::value> docoptReturn;
    void fromDocOpt(docoptReturn& opt);
    void fromConfigFile(const std::string& configFilename);
    void validateArguments();
};

/* Abstract base class for any class that makes use of 
 * the command line arguments. "recieveArguments" is used
 * to pull in arguments from the Args object to itself. */

class ArgumentReciever
{
protected:
    virtual void recieveArguments(const Args&) = 0;
};

}

#endif
