#include "configuration_manager.h"

std::unordered_map<std::string,std::string> ConfigurationManager::configs;


void ConfigurationManager::loadFromFile(const std::string &fileName){
    std::ifstream fileStream(fileName);
    std::string line;
    while(std::getline(fileStream,line)){
        int idx = line.find_first_of('=');
        if(idx == std::string::npos) continue;
        configs[line.substr(0,idx)] = line.substr(idx+1);
    }
}

std::string ConfigurationManager::get(const std::string &key){
    return configs[key];
}
