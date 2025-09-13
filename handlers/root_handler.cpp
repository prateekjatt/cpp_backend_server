#include "root_handler.h"

RootHandler::RootHandler(tcp::socket &&socket):socket(std::move(socket)){
}

void RootHandler::handleRequest(){

}

RootHandler::~RootHandler(){
}