/**
 * @file server.h
 * @author Toprak Efe Akkılıç (efe.akkilic@ozu.edu.tr)
 * @brief 
 * @version 0.1
 * @date 2023-07-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <chrono>
#include <thread>
#include <functional>
#include <ctime>
#include "request.hpp"
#include "cornfield.hpp"

#define IDYLLIB_IP_ADDRESS "127.0.0.1"
#define IDYLLIB_BUFFER_LEN 1048576
#define IDYLLIB_PORT_NUMBR 8080
#define IDYLLIB_HTTP_MAJOR_VERSION "1"
#define IDYLLIB_HTTP_MINOR_VERSION "1"

namespace idyllib {

namespace http {

enum ErrorCode : uint16_t {

    BadRequest = 400,
    NotFound = 404,

};

} /* namespace http*/ 

class Server {
public:  
    Server();    
    ~Server();
    void runServer();
private:
    cornfield::DataBase dataBase;
    int socket_fd;
    struct sockaddr_in *netAddress;
    struct sockaddr_in *createIPv4Address(const char *ip, const int port);
    int createTCPIPv4Socket();
    char *getFileContents(const std::string& filePath);
    void handleConnection(const int client_fd);
    void handleGet(const http::Request &req_in, const int client_fd);
    void sendError(const int client_fd, const http::ErrorCode &error);
    void handlePost(const http::Request &req_in, const int client_fd);
    static std::string getDate();
    static std::string getMime(const std::string &extension);
    static std::string constructHTTPResponse(const std::string &body, const http::Request &req_in);
    static std::string constructHTTPErrorResponse(const http::ErrorCode &error);
};

} /* namespace idyllib*/

#endif