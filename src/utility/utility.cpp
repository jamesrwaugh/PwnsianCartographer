#include "utility.h"
#include <iostream>
#include <fstream>
#ifndef _WIN32
 #include <unistd.h> //for access()
#else
 #include  <io.h>
 #include  <stdio.h>
 #include  <stdlib.h>
#endif

std::vector<std::string> Split(const std::string& target, const std::string& delims)
{
    std::vector<std::string> result;
    size_t startPos = 0, it = 0;
    
    do {
        it = target.find_first_of(delims, startPos);
        result.push_back(target.substr(startPos, it - startPos));
        startPos = target.find_first_not_of(delims, it+1);
    }
    while(it != std::string::npos);
    
    return result;
}

void trim(std::string& s)
{
    s.erase(0, s.find_first_not_of(" \t"));
    s.erase(s.find_last_not_of(" \t")+1, std::string::npos);
}

long getLength(std::istream& is)
{
    long result = 0;

    is.seekg(0, is.end);
    result = is.tellg();
    is.seekg(0, is.beg);

    return result;
}

std::string readStream(std::istream& is)
{
    std::string contents;
    if(is) {
        // get length of file:
        long length = getLength(is);

        //Read into string
        contents.resize(length);
        std::ifstream::char_type* start = &*contents.begin();
        is.read(start, length);
    }
    return contents;
}

std::string readFile(const std::string& filename)
{
    std::ifstream is(filename, std::ios::binary);
    return readStream(is);
}

std::string removePath(const std::string& fullpath)
{
    //Absolute last alnum character in string,
    //and last slash in string, before last alnum chracter
    size_t lastAlphaPos = fullpath.find_last_not_of("\\/");
    size_t firstAlphaPos = fullpath.find_last_of("\\/", lastAlphaPos);

    return fullpath.substr(firstAlphaPos + 1, lastAlphaPos - firstAlphaPos);
}

bool fileExists(const std::string& filename)
{
#ifndef _WIN32
 #define ACCESS access
#else
 #define ACCESS _access
#endif
    return ACCESS(filename.c_str(), F_OK) != -1;
}
