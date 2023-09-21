#include "server.hpp"

int main (int argc, char **argv) {
    std::cout << "Server: Initializing server." << std::endl;
    idyllib::Server mainframe;
    mainframe.runServer();
    return 0;
}