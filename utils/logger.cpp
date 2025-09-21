#include "logger.h"
#include "configuration_manager.h"

Logger *Logger::logger = nullptr;
std::unordered_map<LogType,std::string> Logger::logTypeToString {
    {LogType::DEBUG , "DEBUG"},
    {LogType::ERROR , "ERROR"},
    {LogType::INFO , "INFO"},
};

Logger::Logger(){
    const std::string logFilePath = ConfigurationManager::get("LOG_FILEPATH");
    if(logFilePath.empty()){
        std::cerr << "Error: LOG_FILEPATH environment variables not set!" << std::endl;
        exit(EXIT_FAILURE);
    }

    logFile.open(logFilePath,std::ofstream::app);
    if(!logFile.is_open()){
        std::cerr << "Error: Unable to Open Log File!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

Logger* Logger::getInstance(){
    if(logger == nullptr){
        logger = new Logger();
    }
    return logger;
}

void Logger::log(const LogType &logType,const std::string &message){
    char timestamp[200];
    time_t time = std::time(NULL);
    struct std::tm *tm_st = std::gmtime(&time);
    strftime(timestamp,sizeof(timestamp),"[%Y/%m/%d %H:%M:%S +00:00]",tm_st);

    std::ostringstream logEntry;
    logEntry << timestamp << " [" << Logger::logTypeToString[logType] << "] " << message << std::endl;
    
    std::cout << logEntry.str();

    if(logFile.is_open()){
        logFile << logEntry.str();
        logFile.flush();
    }
}

Logger::~Logger(){
    if(logFile.is_open()){
        logFile.close();
    }
}

