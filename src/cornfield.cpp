#include "cornfield.hpp"

cornfield::DataBase::Bucket::Bucket() {
    next = nullptr;
}

cornfield::DataBase::Bucket::Bucket(const Bucket::Ptr &pbucket_in) {
    next = pbucket_in->next;
    content = pbucket_in->content;
    key = pbucket_in->key;
}

cornfield::DataBase::Bucket::Bucket(const std::string &content_in, const std::string &key_in) {
    next = nullptr;
    content = content_in;
    key = key_in;
}

uint8_t cornfield::DataBase::getHash(const std::string &key_in) {
    uint32_t sum = 0;
    for (std::string::const_iterator iter = key_in.begin(); iter < key_in.end(); iter++) {
        sum += *iter;
    }
    return static_cast<uint8_t> (sum % CORNFIELD_TABLE_SIZE);
}

void cornfield::DataBase::addBucket(const std::string &key_in, const std::string &content_in) {
    /* Retrieve the hash address of the key */
    uint8_t hashAddress = getHash(key_in);
    std::cout << "Database: Adding to element " << key_in << " to index " << (int) hashAddress << '\n';

    /* Check if the hash is initialized, if it isn't then construct a bucket there. */
    if (!hashTable[hashAddress]) {
        hashTable[hashAddress] = static_cast<Bucket::Ptr> (std::make_shared<Bucket>(content_in, key_in));
        return;
    }

    /* Check if the next is null, if it is then place the object there. */
    if (!hashTable[hashAddress]) {
        hashTable[hashAddress]->next = static_cast<Bucket::Ptr> (std::make_shared<Bucket>(content_in, key_in));
        return;
    }

    /* If the first next is full, then navigate towards the last bucket in line and construct a new bucket at the end of the line. */
    Bucket::Ptr currentBucket = hashTable[hashAddress];
    while (currentBucket->next) {
        currentBucket = currentBucket->next;
    }
    currentBucket->next = static_cast<Bucket::Ptr> (std::make_shared<Bucket>(content_in, key_in));
    return;
}

cornfield::DataBase::DataBase() {
    /* Initialize the hashTable to adequate size. */
    hashTable.resize(CORNFIELD_TABLE_SIZE);

    /* Open the database file and construct the hashTable. */
    std::ifstream dataBaseFile;
    dataBaseFile.open(fs::relative(CORNFIELD_DATABASE_PATH, fs::current_path()), std::ios::in);
    
    if (dataBaseFile.is_open()) {
        std::string query;

        /* Read the file line by line and extract name-content values. */
        while (std::getline(dataBaseFile, query)) {
            std::string name;
            std::string content;

            size_t spacePos = query.find(' ');

            if (spacePos != std::string::npos) {
                name = query.substr(0, spacePos);
                content = query.substr(spacePos + 1);
                addBucket(name, content);
            }
        }
        dataBaseFile.close();
    }
}

std::string cornfield::DataBase::retrieveElement(const std::string &key_in) {
    uint8_t hashAddress = getHash(key_in);

    /* Check if the address is initialized. */
    if (!hashTable[hashAddress]) {
        return std::string("");
    }

    /* Check if the first element contains the target. */
     if (hashTable[hashAddress]->key == key_in) {
        return hashTable[hashAddress]->content;

     }

     /* Search through the linked list and serve the file if it is found. */
     Bucket::Ptr currentBucket(hashTable[hashAddress]);
     while (currentBucket->next) {
        currentBucket = currentBucket->next;
        if (currentBucket->key == key_in) {
            return currentBucket->content;
        }
     }

     return std::string("");
}