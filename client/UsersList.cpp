#include <sstream>
#include <iostream>
#include <utility>
#include "UsersList.h"
#include "exceptions.h"

// initialize the users vector.
UsersList::UsersList() {
    list = new std::vector<User>();
}

UsersList::~UsersList() {
    delete list;
}

/**
 * adds user to the list of users. (if not exist)
 * @param name
 * @param uid
 */
void UsersList::addUser(const std::string &name, const bytes &uid) {
    // check if user is in the list
    auto res = std::find_if(list->begin(), list->end(), [&uid](const User &user) { return user.uuid == uid; });
    if (res != list->end())
        return;
    // if user is not in the list, add him to the list of users.
    User u1(name, uid);
    list->push_back(u1);
}


/**
 * returns uuid (if exist)
 * @param username
 * @returns uuid
 */
bytes UsersList::getUUIDbyName(const std::string &username) const {
    auto res = std::find_if(list->begin(), list->end(), [&username](const User &user) { return user.userName == username; });
    if (res != list->end())
        return list->at(std::distance(list->begin(), res)).uuid;
    throw UserIdNotFound();
}

/**
 * returns user name (if exist)
 * @param uid
 * @return
 */
std::string UsersList::getUserNameByUUID(const bytes &uid) {
    auto res = std::find_if(list->begin(), list->end(), [&uid](const User &user) { return user.uuid == uid; });
    if (res != list->end())
        return list->at(std::distance(list->begin(), res)).userName;
    throw UserNameNotFound();
}


/**
 * prints out the list of registered users in the server.
 */
void UsersList::printUserList() {
    std::stringstream st;
    st << "\n\n---------- USER LIST FROM SERVER: ----------\n" << std::endl;
    for (auto &it: *list) {
        st << it.userName << std::endl;
    }
    st << "\n\n" << std::endl;
    std::cout << st.str();
}

/**
 * sets user public key (if exist)
 * @param uid
 * @param pubkey
 */
void UsersList::setPublicKey(const bytes &uid, const bytes &pubkey) {
    auto res = std::find_if(list->begin(), list->end(), [&uid](const User &user) { return user.uuid == uid; });
    if (res != list->end()) {
        list->at(std::distance(list->begin(), res)).publicKey = pubkey;
        return;
    }
    throw UserIdNotFound();
}


/**
 * returns user public key (if exist)
 * @param uid
 * @return user public key
 */
bytes UsersList::getPublicKey(const bytes &uid) {
    auto res = std::find_if(list->begin(), list->end(), [&uid](const User &user) { return user.uuid == uid; });
    if (res != list->end()) {
        auto key = list->at(std::distance(list->begin(), res)).publicKey;
        if (key.empty())
            throw PubKeyNotFound();
        return key;
    }
    throw UserIdNotFound();
}


/**
 * sets user symmetric key (if exist)
 * @param uid
 * @param symKey
 */
void UsersList::setSymmetricKey(const bytes &uid, const bytes &symKey) {
    auto res = std::find_if(list->begin(), list->end(), [&uid](const User &user) { return user.uuid == uid; });
    if (res != list->end()) {
        list->at(std::distance(list->begin(), res)).symKey = symKey;
        return;
    }
    throw UserNotInList();
}

/**
 * returns user s
 * @param uid uid
 * @returns user symmetric key (if exist)
 */
bytes UsersList::getSymmetricKey(const bytes &uid) {
    auto res = std::find_if(list->begin(), list->end(), [&uid](const User &user) { return user.uuid == uid; });
    if (res != list->end()) {
        auto key = list->at(std::distance(list->begin(), res)).symKey;
        if (key.empty())
            throw SymKeyNotFound();
        return key;
    }
    throw UserNotInList();
}

/**
 * Constructor.
 *  makes a new user with name and uuid, with empty public and symmetric keys
 * @param name user name
 * @param id uuid
 */
UsersList::User::User(std::string name, bytes id) {
    userName = std::move(name);
    uuid = std::move(id);
    publicKey = bytes();
    symKey = bytes();

}

