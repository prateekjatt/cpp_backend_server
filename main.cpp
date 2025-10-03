#include "server/server.h"
#include "utils/configuration_manager.h"
#include "utils/logger.h"
#include "db_client/db_client.h"

int main() {
    boost::asio::io_context ioContext;
    
    ConfigurationManager::loadFromFile("server.config");

    Logger *logger = Logger::getInstance();
    
    std::string hostname = ConfigurationManager::get("SERVER_HOSTNAME");
    std::string port = ConfigurationManager::get("SERVER_PORT");
    if(hostname.empty() || port.empty()){
        logger->log(LogType::ERROR,"Unable to find required configuration variables for server!");
        std::exit(EXIT_FAILURE);
    }
    
    std::make_shared<AppServer>(ioContext,hostname,std::atoi(port.c_str()))->start();
    
    DBClient::createInstance(ioContext);
    DBClient::getInstance()->connect();

    ioContext.run();

    DBClient::getInstance()->disconnect();

    return EXIT_SUCCESS;
}