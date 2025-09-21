#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
#include <unordered_map>

enum LogType {
    INFO,
    DEBUG,
    ERROR,
};

class Logger{
private:
    std::ofstream logFile;
    Logger();
    static std::unordered_map<LogType,std::string> logTypeToString;
    static Logger *logger;
public:
    static Logger* getInstance();
    void log(const LogType &logType,const std::string &message);
    ~Logger();
};

#endif