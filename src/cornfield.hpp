/**
 * @file cornfield.h
 * @author Toprak Efe Akkılıç (efe.akkilic@ozu.edu.tr)
 * @brief 
 * @version 0.1
 * @date 2023-07-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CORNFIELD_H
#define CORNFIELD_H

#include <array>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdint.h>

#define CORNFIELD_DATABASE_PATH "/home/toprak-efe/Documents/idyllib_ws/database/site.cfdb"
#define CORNFIELD_TABLE_SIZE 256

namespace fs = std::filesystem;
namespace cornfield {    

class DataBase {
private:

    typedef struct Bucket {
        //Pointer and Constant Pointer type declarations
        typedef std::shared_ptr<struct Bucket> Ptr;

        //Member declarations
        Bucket::Ptr next;       //Pointer to the next bucket.
        std::string content;    //Content of the entry.
        std::string key;        //Name of the entry.

        Bucket();
        Bucket(const Bucket::Ptr &pbucket_in);
        Bucket(const std::string &content_in, const std::string &key_in);

    } Bucket;

    std::vector<Bucket::Ptr> hashTable;                                           //Stores the actual hash table.
    static uint8_t getHash(const std::string &key_in);                            //Generates an 8-bit key to be used for hash indexing.
    void addBucket(const std::string &key_in, const std::string &content_in);     //Adds the input to the hashTable, creates/extends a linked list it if overlap occurs.
public:
    DataBase();
    /**
     * @brief Retrieve a string containing the file contents by its name.
     * 
     * @param key_in filename
     * @return std::string 
     * @note Returns empty string if the file doesn't exist.
     */
    std::string retrieveElement(const std::string &key_in);
};

} // namespace cornfield

#endif /* CORNFIELD_H */