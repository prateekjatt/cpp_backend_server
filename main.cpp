#include "server/server.h"
#include "configuration_manager/configuration_manager.h"

int main() {
    ConfigurationManager::loadFromFile("server.config");

    std::string hostname = ConfigurationManager::get("SERVER_HOSTNAME");
    std::string port = ConfigurationManager::get("SERVER_PORT");
    if(hostname.empty() || port.empty()){
        std::cout << "ERROR: Unable to find required configuration variables for server!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::make_shared<AppServer>(hostname,std::atoi(port.c_str()))->start();
    return EXIT_SUCCESS;
}