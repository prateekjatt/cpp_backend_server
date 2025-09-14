#include "db_client.h"
#include "../configuration_manager/configuration_manager.h"

DBClient::DBClient(boost::asio::io_context &ioContext):conn(ioContext){

    std::string hostname = ConfigurationManager::get("DB_HOSTNAME");
    std::string port = ConfigurationManager::get("DB_PORT");
    std::string username = ConfigurationManager::get("DB_USERNAME");
    std::string password = ConfigurationManager::get("DB_PASSWORD");

    if(hostname.empty() || port.empty() || username.empty() || password.empty()){
        std::cout << "ERROR: Unable to find required environment variables for database!" << std::endl;
        std::exit(EXIT_FAILURE);
    } 
    this->hostname = hostname;
    this->port = std::atoi(port.c_str());
    this->username = username;
    this->password = password;
}

void DBClient::connect(){
    boost::mysql::connect_params params;
    params.server_address.emplace_host_and_port(hostname,port);
    params.username = username;
    params.password = password;
    conn.connect(params);

    std::cout << "INFO: Database Connected Successfully" << std::endl;
}

void DBClient::execute(){
    boost::mysql::results result;
    conn.execute("select 'C++ Web Server!'",result);
    std::cout << "INFO: Query Executed: " << result.rows().at(0).at(0).as_string() << std::endl;
}

void DBClient::disconnect(){
    conn.close();
}

DBClient::~DBClient(){
}