#include "utilities.h"


std::unordered_map<std::string,std::string> Utilities::parseCookies(std::string cookies){
    std::unordered_map<std::string,std::string> res;

    std::stringstream ss(cookies);

    std::string cookie;
    while(std::getline(ss,cookie,';')){
        if(cookie.find_first_of('=') != std::string::npos){
            int idx = cookie.find_first_of('=');
            res[cookie.substr(0,idx)] = cookie.substr(idx+1);
        }
    }

    return res;
}