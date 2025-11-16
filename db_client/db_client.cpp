#include "db_client.h"
#include "../utils/configuration_manager.h"
#include "../utils/globals.h"

DBClient *DBClient::dbClient = nullptr;

DBClient::DBClient():conn_pool(Globals::ioContext,{}){

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

    boost::mysql::pool_params params;
    params.server_address.emplace_host_and_port(hostname,std::atoi(port.c_str()));
    params.username = username;
    params.password = password;
    params.database = database;
    conn_pool = boost::mysql::connection_pool(Globals::ioContext,std::move(params));
    conn_pool.async_run(boost::asio::detached);
}

DBClient* DBClient::getInstance(){
    if(dbClient == nullptr){
        dbClient = new DBClient();
    }
    return dbClient;
}

boost::asio::awaitable<boost::mysql::pooled_connection> DBClient::getConnection(){
    co_return co_await dbClient->conn_pool.async_get_connection(boost::asio::cancel_after(std::chrono::seconds(5)));
}

void DBClient::disconnect(){
    conn_pool.cancel();
}

DBClient::~DBClient(){
    disconnect();
}