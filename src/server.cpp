#include "server.hpp"

int idyllib::Server::createTCPIPv4Socket() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        std::cerr << "Server: Socket creation failed.\n";
        return -1;
    }

    const int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Server: Socket initialization failed.\n";
        return -1;
    }

    return socket_fd;
}

struct sockaddr_in *idyllib::Server::createIPv4Address(const char *ip, const int port) {
    
    struct sockaddr_in *net_address_out = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
    net_address_out->sin_port = htons(port);
    net_address_out->sin_family = AF_INET;
    
    if (strlen(ip) == 0) {
        net_address_out->sin_addr.s_addr = INADDR_ANY;
    }
    
    else {
        inet_pton(AF_INET, ip, &net_address_out->sin_addr.s_addr);
    }

    return net_address_out;
}

char *idyllib::Server::getFileContents(const std::string& filePath) {
    /* Open the file in binary read mode. */
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return nullptr;
    }

    /* Seek to the end of th*/
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[fileSize];
    file.read(buffer, fileSize);
    file.close();

    return buffer;
}

void idyllib::Server::handleConnection(const int client_fd) {
    char *msgBuffer = nullptr;
    while (msgBuffer == nullptr) {
        msgBuffer = (char *) malloc(IDYLLIB_BUFFER_LEN);
    }
    
    ssize_t bytesReceived = recv(client_fd, msgBuffer, IDYLLIB_BUFFER_LEN-1, 0);

    if (bytesReceived > 0) {
        std::string requestMessage(msgBuffer);
        free(msgBuffer);

        /* Check if the entire header was sent gracefully, otherwise, terminate. */
        size_t headerTerminatePos = requestMessage.find("\r\n\r\n");
        if (headerTerminatePos == std::string::npos) {
            close(client_fd);
            return;
        }

        /* Parse the HTTP and handle the request. */
        http::Request clientRequest(requestMessage);
        switch (clientRequest.http_meth) {
        case (http::Get):
            handleGet(clientRequest, client_fd);
            break;
        case (http::Post):
            handlePost(clientRequest, client_fd);
            break;
        }

        close(client_fd);
        return;
    }

    free(msgBuffer);
    close(client_fd);
    return;
}

idyllib::Server::Server() {
    socket_fd = createTCPIPv4Socket();
    netAddress = createIPv4Address(IDYLLIB_IP_ADDRESS, IDYLLIB_PORT_NUMBR);
}   

idyllib::Server::~Server() {
    close(socket_fd);
    free(netAddress);
}

void idyllib::Server::runServer() {

    if (bind(socket_fd, (struct sockaddr *) netAddress, sizeof(*netAddress)) < 0) {
        std::cerr << "Server: Binding failed, shutting down.\n";
        return;
    }

    if (listen(socket_fd, 1) < 0) {
        std::cerr << "Server: Listening failed, shutting down.\n";
        return;
    }

    struct sockaddr_in clientAddress;
    int address_size = sizeof(clientAddress);
    while (true) {

        int client_socket_fd;
        if (client_socket_fd = accept(socket_fd, (struct sockaddr *) &clientAddress, (socklen_t *) &address_size) < -1) {
            std::cerr << "Server: Accepting failed, listening for connections.\n";
            continue;
        }

        handleConnection(client_socket_fd);

    }
}

std::string idyllib::Server::getMime(const std::string &extension) {
    static std::unordered_map<std::string, std::string> mimeMap({
        {".html", "text/html"},
        {".htm", "text/html"},
        {".css", "text/css"},
        {".js", "text/javascript"},
        {".ico", "image/vnd.microsoft.icon"},
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".gif", "image/gif"},
        {".bmp", "image/bmp"},
        {".pdf", "application/pdf"}
    }); 
                                                     
    return mimeMap[extension];
}

void idyllib::Server::handleGet(const http::Request &req_in, const int client_fd) {
    /* Retrieve the file contents into a string from the database. */
    std::string filePath = dataBase.retrieveElement(req_in.link);
    char *buffer = getFileContents(filePath);
    if (buffer == nullptr) {
        std::cerr << "Server: Requested file " << req_in.link << " does not exist.\n";
        sendError(client_fd, http::ErrorCode::NotFound);
        return;
    }
    std::string fileBody(buffer);
    free(buffer);

    std::string response = constructHTTPResponse(fileBody, req_in);
    send(client_fd, response.c_str(), response.length(), 0);
    std::cerr << "Server: Sending file: " << filePath << "\nTo host: " << req_in.host << "\n"; 
}

void idyllib::Server::handlePost(const http::Request &req_in, const int client_fd) {
    return;
}

std::string idyllib::Server::getDate() {
    /* Initialize the date. */
    std::string dateOut;
    std::time_t rawtime;
    std::tm* timeinfo;
    char buffer[80];
    std::time(&rawtime);
    timeinfo = std::gmtime(&rawtime);
    if (timeinfo) {
        std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
        dateOut += "Date: ";
        dateOut += buffer;
        dateOut += "\r\n";
    }

    return dateOut;
}

std::string idyllib::Server::constructHTTPErrorResponse(const idyllib::http::ErrorCode &error) {
    static std::unordered_map<http::ErrorCode, std::string> mimeMap({
        {http::ErrorCode::BadRequest, "400 Bad Request"},
        {http::ErrorCode::NotFound, "404 Not Found"}
    });

    std::string errorOut;
    errorOut += "HTTP/";
    errorOut += IDYLLIB_HTTP_MAJOR_VERSION;
    errorOut += ".";
    errorOut += IDYLLIB_HTTP_MINOR_VERSION;
    errorOut += " ";
    errorOut += mimeMap[error];
    errorOut += "\r\n";
    errorOut += getDate();
    errorOut += "Server: Idyllic/1.0 (Linux)\r\n";
    
    return errorOut;

}

void idyllib::Server::sendError(const int client_fd, const idyllib::http::ErrorCode &error) {
    std::string response = constructHTTPErrorResponse(error);
    send(client_fd, response.c_str(), response.length(), 0);
}

std::string idyllib::Server::constructHTTPResponse(const std::string &body, const http::Request &req_in) {
    std::string responseOut;
    /* Initialize the first line. */
    responseOut += "HTTP/";
    responseOut += IDYLLIB_HTTP_MAJOR_VERSION;
    responseOut += ".";
    responseOut += IDYLLIB_HTTP_MINOR_VERSION;
    responseOut += " 200 OK\r\n";

    /* Initialize the date. */
    responseOut += getDate();

    /* Initialize the server field. */
    responseOut += "Server: Idyllic/1.0 (Linux)\r\n";

    /* Initialize the content length. */
    responseOut += "Content-Length: ";
    responseOut += std::to_string(body.length());
    responseOut += "\r\n";

    /* Set the connection to close. */
    responseOut += "Connection: Closed\r\n";

    /* Initialize the content type. */
    responseOut += "Content-Type: ";
    responseOut += getMime(req_in.fext);
    responseOut += "\r\n\r\n";

    /* Append the body. */
    responseOut += body;

    return responseOut;
}