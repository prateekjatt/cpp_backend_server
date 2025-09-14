#ifndef _CONFIGURATION_MANAGER_H_
#define _CONFIGURATION_MANAGER_H_

#include <unordered_map>
#include <iostream>
#include <fstream>

class ConfigurationManager{
private:
    static std::unordered_map<std::string,std::string> configs;
    ConfigurationManager(){}
public:
    static void loadFromFile(const std::string &fileName);
    static std::string get(const std::string &key);
};

#endif