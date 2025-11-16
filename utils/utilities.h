#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <sstream>
#include <unordered_map>

namespace Utilities {
    std::unordered_map<std::string,std::string> parseCookies(std::string cookies);
}

#endif