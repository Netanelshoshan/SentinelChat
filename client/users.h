/**
 * This class is responsible for representing the users.
 * Adding a new user to the list of registered user in the server
 * getting and setting public and symmetric key for a user and
 * getting the list of current registered users.
 */
#ifndef USERSLIST_HEADER
#define USERSLIST_HEADER

#include <vector>
#include <iterator>
#include <algorithm>
#include <string>
#include "constants.h"

class users {
private:
    class User {
    public:
        std::string userName;
        bytes uuid;
        bytes publicKey;
        bytes symKey;
        User(std::string name, bytes id); // for making new user.
    };

    std::vector<User> *list; // the list of users ( "friends" ) that are registered in the server.

public:
    users();
    ~users();
    //adds user to the list of users. (if not exist)
    void addUser(const std::string &name, const bytes &uid);

    // returns username (if exist)
    bytes getUUIDbyName(const std::string &name) const;

    //returns username (if exist)
    std::string getUserNameByUUID(const bytes &uid);

    // prints out the list of registered users in the server.
    void printUserList();

    //sets user public key (if exist)
    void setPublicKey(const bytes &uid, const bytes &pubkey);

    //sets user skey if exist
    void setSymmetricKey(const bytes &uid, const bytes &symkey);

    //returns user public key (if exist)
    bytes getPublicKey(const bytes &uid);

    //returns user skey if exist
    bytes getSymmetricKey(const bytes &uid);
};

#endif