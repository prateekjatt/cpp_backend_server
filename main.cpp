#include "server/server.h"

int main() {
    std::cout << "Instansiating Server\n";
    std::cout << "Starting Server\n";
    std::make_shared<AppServer>(3000)->start();
    std::cout << "Closing Server" << std::endl;
    return 0;
}