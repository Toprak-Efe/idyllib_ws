/**
 * @file parsing.h
 * @author Toprak Efe Akkılıç (efe.akkilic@ozu.edu.tr)
 * @brief 
 * @version 0.1
 * @date 2023-07-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <iostream>
#include <unordered_map>

namespace idyllib {

namespace http {

enum Method : uint8_t {

    InvalidMethod = 0,
    Get = 1,
    Post = 2

};

/**
 * @brief Utility struct to parse HTTP with.
 * 
 */
struct Request {
    uint32_t body_len;      /* Length of the request body.*/
    uint8_t vers_maj;       /* HTTP major version. */
    uint8_t vers_min;       /* HTTP minor version. */
    Method http_meth;       /* Type of HTTP request. */
    std::string fext;       /* File extension.*/
    std::string host;       /* Host name if it is included. */
    std::string link;       /* Request target URI. */
    std::string contents;   /* Post request body. */
    Request(const std::string &buffer);
};

} /* namespace http */

} /* namespace idyllib*/

#endif