#include "request.hpp"

idyllib::http::Request::Request(const std::string &requestBuffer) {
    static std::unordered_map<std::string, Method> methodDict({{"GET", Get},
                                                               {"POST", Post}});

    std::string requestMsg(requestBuffer);
    std::string endHeader = "\r\n\r\n";
    std::string endLine = "\r\n";
    size_t uriPos;
    size_t verPos;
    size_t endPos;

    /* Retrieve the end index of the first line. */
    endPos = requestMsg.find(endLine);

    /* Retrieve the indices of the two spaces. */
    uriPos = requestMsg.find(' ');
    verPos = requestMsg.find(' ', uriPos+1);

    /* Set the method member. */
    std::string methodStr = requestMsg.substr(0, uriPos);
    try {
        http_meth = methodDict.at(methodStr);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        std::cerr << "Server: Error in reading HTTP method, expected GET or POST but received " << methodStr << " instead." << '\n';
        http_meth = Get; //Default value.
    }
    
    /* Retrieve the link. */
    std::string linkStr = requestMsg.substr(uriPos+2, verPos - uriPos - 2);
    link = linkStr;

    /* Find the file extension. */
    size_t dotPos = linkStr.find('.');
    std::string extStr = linkStr.substr(dotPos);
    fext = extStr;

    { /* Setup the version numbers. */
    size_t dotPos = requestMsg.rfind('.', endPos);
    vers_maj = static_cast<uint8_t>(requestMsg[dotPos-1] - '0');
    vers_min = static_cast<uint8_t>(requestMsg[dotPos+1] - '0');
    } /* Setup the version numbers. */

    { /* Retrieve the host header if it exists. */
    std::string hostHeader("\r\nHost: ");
    size_t hostPos = requestMsg.find(hostHeader) + hostHeader.length();
    if (hostPos != std::string::npos) {
        size_t hostEndPos = requestMsg.find(endLine, hostPos);
        host = requestMsg.substr(hostPos, hostEndPos - hostPos - 1);
    }
    } /* Retrieve the host header if it exists. */

    /* Initialize HTTP Body*/
    if (http_meth == Get) {
        contents = "";
        body_len = 0;
        return;
    }
    
    { /* Retrieve content-length header. */    
    std::string lenHeader("\r\nContent-Length: ");
    size_t lenPos = requestMsg.find(lenHeader) + lenHeader.length();
    if (lenPos != std::string::npos) {
        size_t lenEndPos = requestMsg.find(endLine, lenPos);
        body_len = std::stoi(requestMsg.substr(lenPos, lenEndPos - lenPos - 1));
    }
    } /* Retrieve content-length header. */

    /* Retrieve message body. */
    size_t bodyPos = requestMsg.find(endHeader) + endHeader.length();
    contents = requestMsg.substr(bodyPos);
}