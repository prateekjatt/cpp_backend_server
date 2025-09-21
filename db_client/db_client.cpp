#include "db_client.h"
#include "../utils/configuration_manager.h"

DBClient *DBClient::dbClient = nullptr;

DBClient::DBClient(boost::asio::io_context &ioContext):conn(ioContext){

    logger = Logger::getInstance();

    std::string hostname = ConfigurationManager::get("DB_HOSTNAME");
    std::string port = ConfigurationManager::get("DB_PORT");
    std::string username = ConfigurationManager::get("DB_USERNAME");
    std::string password = ConfigurationManager::get("DB_PASSWORD");
    std::string database = ConfigurationManager::get("DB_NAME");

    if(hostname.empty() || port.empty() || username.empty() || password.empty()){
        logger->log(LogType::ERROR,"Unable to find required environment variables for database!");
        std::exit(EXIT_FAILURE);
    } 
    this->hostname = hostname;
    this->port = std::atoi(port.c_str());
    this->username = username;
    this->password = password;
    this->database = database;
}

void DBClient::createInstance(boost::asio::io_context &ioContext){
    if(dbClient == nullptr){
        dbClient = new DBClient(ioContext);
    }
}

DBClient* DBClient::getInstance(){
    return dbClient;
}

void DBClient::connect(){
    boost::mysql::connect_params params;
    params.server_address.emplace_host_and_port(hostname,port);
    params.username = username;
    params.password = password;
    params.database = database;
    conn.connect(params);

    logger->log(LogType::INFO,"Database Connected Successfully");
}

boost::mysql::results DBClient::execute(const auto &query){
    boost::mysql::results result;
    conn.execute(query,result);
    return result;
}

void DBClient::disconnect(){
    conn.close();
}

DBClient::~DBClient(){
    disconnect();
}