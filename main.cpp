#include "server/server.h"
#include "utils/configuration_manager.h"
#include "utils/logger.h"
#include "db_client/db_client.h"
#include <sodium.h>
#include "utils/cache_storage.h"
#include <boost/redis/src.hpp>

int main() {
    boost::asio::io_context ioContext;
    
    ConfigurationManager::loadFromFile("server.config");

    Logger *logger = Logger::getInstance();

    logger->log(LogType::DEBUG,"Configuration File Loaded");

    std::string hostname = ConfigurationManager::get("SERVER_HOSTNAME");
    std::string port = ConfigurationManager::get("SERVER_PORT");
    if(hostname.empty() || port.empty()){
        logger->log(LogType::ERROR,"Unable to find required configuration variables for server!");
        std::exit(EXIT_FAILURE);
    }
    
    if(sodium_init() < 0){
        logger->log(LogType::ERROR,"Unable to Initilization Sodium!");
        std::exit(EXIT_FAILURE);
    }
    
    std::make_shared<AppServer>(ioContext,hostname,std::atoi(port.c_str()))->start();
    
    DBClient::createInstance(ioContext);
    DBClient::getInstance()->connect();

    CacheStorage::createInstance(ioContext);

    ioContext.run();

    DBClient::getInstance()->disconnect();

    return EXIT_SUCCESS;
}