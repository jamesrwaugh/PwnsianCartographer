#ifndef UTILITY_H
#define UTILITY_H
#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <sstream>
#include <iostream>

/* Improved Split funtion that skips over consecutive delimiters, 
 * and splits on more than one delimiter (Used for space and tab) */
std::vector<std::string> Split(const std::string& target, const std::string& delims);

//Remove whitespace before and after a string
void trim(std::string& s);

//roger - use this for printing a string on a single line
template <typename printType>
void println(printType const &str)
{ 
    std::cout<<str<<std::endl; 
}

template <typename printType>
void printVector(std::vector<printType> const &elements)
{
    for(const auto& e : elements) { println(e); }
}

//Provides a << operator for any standard container
template<typename T, class = decltype(std::declval<T>().begin())>
typename std::enable_if<!std::is_same<T,std::string>::value, std::ostream&>::type
operator<<(std::ostream& os, const T& cntr)
{
    os << '[';
    if(!(cntr.empty())) {
        auto end = std::prev(cntr.end());
        for(auto it = cntr.begin(); it != end; ++it)
            os << *it << ",";
        os << *end;
    }
    os << ']';

    return os;
}

//Used to format strings into a runtime_error.
//See http://en.cppreference.com/w/cpp/language/parameter_pack
template<typename... Ts>
void error(Ts... args)
{
    std::stringstream ss;
    int dummy[sizeof...(Ts)] = { (ss << args, 0)... };
    (void)dummy;
    throw std::runtime_error(ss.str());
}

//Used to format strings into a debug message.
//See http://en.cppreference.com/w/cpp/language/parameter_pack
template<typename... Ts>
void log(Ts... args)
{
    std::stringstream ss;
    int dummy[sizeof...(Ts)] = { (ss << args, 0)... };
    (void)dummy;
    std::cout << ss.str() << std::endl;
}

//Get the length of a stream, usually a file
long getLength(std::istream& is);

//Loads and returns the entire contents of a stream
std::string readStream(std::istream &is);

//Loads and returns the entire contents of a file in a string
std::string readFile(const std::string& filename);

//Return part of a path after the last slash, but before any trailing slashes (i.e, filename)
std::string removePath(const std::string& fullpath);

//Does a file exists?
bool fileExists(const std::string& filename);

#endif
