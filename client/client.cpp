/**
 * Author : Netanel Shoshan
 * This class is responsible all requests and responses and IO from
 * and to the client.
 */
#include <iostream>
#include <fstream>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "client.h"
#include "exceptions.h"
#include "utils.h"

/**
 * 1. Initializes the request and the users vectors.
 * 2. Initializes the crypto_handler that will load
 *    the private key from me.info or will init. one.
 */
client::client() {
    request = new requests();
    usersList = new users();
    keys = new crypto_handler();
}

client::~client() {
    delete request;
    delete usersList;
    delete keys;
}

/**
 * IO loop. Handles user requests.
 * @return true while user keeps using the client.
 */
bool client::io() {
    int selection;
    bool valid = false;

    // if me.info exist - that means that user already registered
    bool registered = boost::filesystem::exists(ME_INFO);

    //if user is registered
    if (registered) {
        getUUID();// get his uuid
        keys->getPrivateKey();//load his private key from the file.
    }

    /**
     * loop until client provides input and validate it.
     * */
    while (!valid) {
        printMenu();// pretty print the menu.
        selection = getUserSelection(); // get user selection.
        switch (selection) {
            case (EXIT):
                return false;
            case (INVALID_INPUT):
                std::cout << "Invalid option." << std::endl;
                break;
            case (REG):
                if (registered) {
                    std::cout << "You're already registered." << std::endl;
                    break;
                }
                else {
                    /** opcode 110 - client isn't registered. build register request
                    * returns true if request if valid, false otherwise.
                   */
                    valid = requestBuilder(selection);
                    break;
                }
            default:
                /** for all other options, check if user is registered and pass on
                    his selection */
                if (!registered) {
                    std::cout << "You're not registered. Please register." << std::endl;
                    break;
                }
                else
                    valid = requestBuilder(selection);
        }
    }
    return true;
}

/**
 * @return the request vector
 */
const bytes &client::getRequest() {
    return request->getRequest();
}

/**
 * Prints out the menu with avaliable options for the client.
 */
void client::printMenu() {
    std::cout << std::endl;
    std::cout << "SentinelChat client at your service." << std::endl << std::endl;
    std::cout << "110) Register" << std::endl;
    std::cout << "120) requests for clients list" << std::endl;
    std::cout << "130) requests for public key" << std::endl;
    std::cout << "140) requests for waiting messages" << std::endl;
    std::cout << "150) Send a text message" << std::endl;
    std::cout << "151) Send a requests for symmetric key" << std::endl;
    std::cout << "152) Send your symmetric key" << std::endl;
    std::cout << "153) Send a file" << std::endl;
    std::cout << "0) Exit client" << std::endl;
    std::cout << "?" << std::endl;
}

/**
 * gets user selection code.
 * validate that input is all digits.
 * @return user selection code.
 */
int client::getUserSelection() {
    std::string input;
    std::getline(std::cin, input);
    if (input.empty())
        return INVALID_INPUT;

    // validate input contains only numbers.
    for (char i: input)
        if (!std::isdigit(i))
            return INVALID_INPUT;

    // convert it.
    int selection = std::stoi(input);

    // return user selection.
    switch (selection) {
        case (REG):
        case (GET_USERLIST):
        case (GET_PUBLIC_KEY):
        case (GET_PENDING_MSGS):
        case (SEND_MSG):
        case (GET_SYMKEY):
        case (SEND_SYMKEY):
        case (SEND_FILE):
        case (EXIT):
            return selection;
        default:
            return INVALID_INPUT;
    }
}


/**
 * build the request vector based on client's selection.
 * @param selection
 * @return true if request was built successfully.
 */
bool client::requestBuilder(int selection) {
    // Builds the header up to payload_size field.
    request->buildHeader(getUUID(), getReqCode(selection));

    // Builds and push the payload based on user selection.
    switch (selection) {
        case (REG): {// opcode 110 - register with name and pubkey.
            this->userName = getUserName();
            bytes uname_asBytes(this->userName.begin(), this->userName.end());
            request->registerRequest(uname_asBytes, keys->generatePublicKey());
            return true;

        }
        case (GET_USERLIST): {// opcode 120 - get the list of users. (no payload)
            request->getUserListRequest();
            return true;
        }
        case (GET_PUBLIC_KEY): {
            request->pushInt(UID_SIZE);
            std::string dstUserName = getUserName();
            try {
                // need to check if there is public key for the protocol?
                bytes userID = usersList->getUUIDbyName(dstUserName);
                request->getPublicKeyRequest(userID);
                return true;
            }
            catch (std::exception &e) {
                std::cout << e.what() << std::endl;
                return false;
            }
        }
        case (GET_PENDING_MSGS): {// opcode 140 - get pending messages.(no payload)
            request->getPendingMessages();
            return true;
        }
            /**
                 * opcodes 150-153
                 */
        case (SEND_FILE):
        case (GET_SYMKEY):
        case (SEND_MSG):
        case (SEND_SYMKEY): {
            string dst_username, message, filePath;
            bytes dst_uuid, encodedMessage;
            int payloadSize = 0, messageSize = 0;

            unsigned char messageType;

            dst_username = getUserName();
            try {
                // get user id. will throw error if user isn't in users list
                dst_uuid = usersList->getUUIDbyName(dst_username);
                switch (selection) {
                    /**
                     * opcode 150. messageType = 3
                     */
                    case (SEND_MSG):
                        messageType = MSG_TYPE_SEND_MSG;
                        encodedMessage = keys->encryptAES(usersList->getSymmetricKey(dst_uuid),
                                                          getMessage());
                        break;
                        /**
                          * opcode 153 - send my symmetric key, messageType = 4.
                         */
                    case (SEND_FILE):
                        messageType = MSG_TYPE_SEND_FILE;
                        encodedMessage = keys->encryptAES(usersList->getSymmetricKey(dst_uuid),
                                                          loadFile(getFilePath()));
                        break;
                        /**
                       * opcode 151 - get symmetric key. messageType = 1 .
                       */
                    case (GET_SYMKEY):
                        messageType = MSG_TYPE_GET_SYM_KEY;
                        break;
                        /**
                        * opcode 152 - send my symmetric key, messageType = 2.
                        */
                    case (SEND_SYMKEY):
                        messageType = MSG_TYPE_SEND_SYM_KEY;
                        usersList->setSymmetricKey(dst_uuid,
                                                   keys->generateSymmetricKey());    // make new symmetric key and save it.
                        encodedMessage = keys->encryptRSA(usersList->getPublicKey(dst_uuid),
                                                          usersList->getSymmetricKey(dst_uuid));
                        break;
                    default:
                        return false;
                }

                messageSize = (int) encodedMessage.size();
                payloadSize = UID_SIZE + 1 + sizeof(int) + messageSize;

                // build the message payload + payload size field.
                request->sendMessageRequest(payloadSize, dst_uuid, messageType, messageSize, encodedMessage);
                return true;
            }
            catch (std::exception &e) {
                std::cout << e.what() << std::endl;
                return false;
            }

        }    // end send message case
        default:
            return false;

    }
}

// returns requests code of the given user selection
uint16_t client::getReqCode(int selection) {
    switch (selection) {
        case (REG):
            return REG_REQ;
        case (GET_USERLIST):
            return GET_USER_LIST_REQ;
        case (GET_PUBLIC_KEY):
            return GET_PUBKEY_REQ;
        case (GET_PENDING_MSGS):
            return GET_MSG_REQ;
        case (SEND_MSG):
        case (GET_SYMKEY):
        case (SEND_SYMKEY):
        case (SEND_FILE):
            return SEND_MSG_REQ;
        default:
            return INVALID_INPUT;
    }
}

/**
 * Gets the desired username from the client.
 * @return username
 */
std::string client::getUserName() {
    std::string user_name;
    bool valid = false;
    while (!valid) {
        std::cout << "Please enter user name: " << std::endl;
        std::getline(std::cin, user_name);
        if (user_name.empty() || user_name.size() > MAX_USERNAME_LEN)
            std::cout << "invalid user name size." << std::endl;
        else
            valid = true;
    }
    return user_name;
}


/**
 * gets the message the client want to sent
 * @return the message
 */
bytes client::getMessage() {
    std::string message;
    std::cout << "What's you're message ? " << std::endl;
    std::getline(std::cin, message);
    bytes m(message.begin(), message.end());
    return m;
}


/**
 * Gets the response and saves userName, uuid
 * and the user's private key into me.info.
 * - This function will be called on successful user registration. -
 * @param response
 */
void client::saveUserInfo(const bytes &response) {
    std::fstream infoFile(ME_INFO, std::fstream::out | std::fstream::app);
    string uuid(response.begin(), response.end());

    // convert uuid to hex string and write it into me.info
    string uuid_asHex;
    boost::algorithm::hex(uuid.begin(), uuid.end(), std::back_inserter(uuid_asHex));

    infoFile << this->userName << '\n' << uuid_asHex << '\n';
    infoFile.close();
    keys->savePrivateKey();
}


/**
 * Gets the response from the server and parse it according to
 * the response code field
 * @param response
 */
void client::responseParser(responses *response) {
    if (response->getServerVersion() != SERVER_VERSION)
        throw IncompatibleServerVersion();

    switch (response->getResponseCode()) {
        case (INTERNAL_ERROR):
            if (request->getRequestCode())
                std::cout << "User name is already taken. Please try again with another name." << std::endl;
            else
                std::cout << "Internal Error occurred." << std::endl;
            break;
            /**
             * On successful registration, save username, UUID and private key into me.info.
             * will throw an error if user isn't registered .
             */
        case (REG_SUCC):// code 2100 - register success.
            try {
                saveUserInfo(response->getPayload());
            }
            catch (std::exception &e) {
                std::cout << e.what() << std::endl << "You're not registered. Please register first." << std::endl;
                // Delete created me.info if error occurred.
                if (boost::filesystem::exists(ME_INFO))
                    boost::filesystem::remove(ME_INFO);
            }
            break;
        case (GET_USER_LIST_SUCC):// code 2101 - get user list success.
            generateUserList(response->getPayload());
            usersList->printUserList();
            break;
        case (GET_PUB_KEY_SUCC):
            keys->setPublicKey(*usersList, response->getPayload());
            break;
        case (SEND_MSG_SUCC):
            std::cout << "Message sent." << std::endl;
            break;
        case (GET_MSG_SUCC):
            printMessages(response->getPayload());
    }
}

/**
 * generates the userlist vector with all the users exclude me.
 * @param payload the payload from the server.
 */
void client::generateUserList(const bytes &payload) {
    // as suggested.
    int numOfUsers = (int) payload.size() / (UID_SIZE + MAX_USERNAME_LEN);

    for (int i = 0; i < numOfUsers; i++) {
        size_t start = i * (UID_SIZE + MAX_USERNAME_LEN);
        size_t current = start;

        // read the first 16bytes and push them to uuid vec.
        bytes uuid;
        while (current < start + UID_SIZE)
            uuid.push_back(payload.at(current++));

        string username;

        /**
         * read username bytes and push them to username string.
            */
        while (current < payload.size() && payload.at(current))
            username.push_back(payload.at(current++));
        usersList->addUser(username, uuid);
    }
}

/**
 * pretty print the messages we got from the server.
 * @param payload
 */
void client::printMessages(const bytes &payload) {
    size_t index = 0;
    while (index < payload.size()) {
        bytes fromUser;
        size_t start = index;

        /**
        * parse each message from the payload
        * and act accordingly
        */
        while (index < start + UID_SIZE)
            fromUser.push_back(payload.at(index++));


        index += 4; // skip message id (we don't need it)

        int messageType = payload.at(index++); // read messageType

        //read message size and convert it to integer.
        string msg_size;
        for (int i = 0; i < 4; i++)
            msg_size += payload.at(index++);

        // convert temp variable to integer in this machines endianess
        int messageSize = bytesToInt((const unsigned char *) msg_size.c_str());

        // read messageSize bytes into conetent vector
        bytes content;
        for (int i = 0; i < messageSize; i++) {
            content.push_back(payload.at(index++));
        }

        try {
            std::cout << "from:  " << std::endl;
            std::cout << usersList->getUserNameByUUID(fromUser) << std::endl;
            std::cout << "content:" << std::endl;

            // try to parse it.
            switch (messageType) {
                case (MSG_TYPE_GET_SYM_KEY): {
                    std::cout << "requests For symmetric Key." << std::endl;
                    break;
                }
                case (MSG_TYPE_SEND_SYM_KEY): {
                    usersList->setSymmetricKey(fromUser, keys->decryptRSA(content));
                    std::cout << "Symmetric key received." << std::endl;
                    break;
                }
                case (MSG_TYPE_SEND_MSG): {
                    bytes key = usersList->getSymmetricKey(fromUser);
                    bytes decodedMsg = keys->decryptAES(key, content);
                    std::cout << string(decodedMsg.begin(), decodedMsg.end()) << std::endl;
                    break;
                }
                case (MSG_TYPE_SEND_FILE): {
                    bytes key = usersList->getSymmetricKey(fromUser);
                    try {
                        bytes decodedMsg = keys->decryptAES(key, content);
                        std::cout << "File was saved to:\n";
                        std::cout << saveFile(decodedMsg) << std::endl;

                    }
                    catch (std::exception &e) {
                        std::cout << "printMessages(): Can't save file" << std::endl << e.what() << std::endl;
                    }
                }
            }
        }
        catch (KeyNotAvailable &e) {
            std::cout << "can't decrypt message\n" << e.what() << std::endl;
        }
        catch (UserNotInList &e) {
            std::cout << "can't decrypt message\n" << e.what() << std::endl;
        }
        catch (std::exception &e) {
            std::cout << e.what() << std::endl;
        }

        std::cout << std::endl << "-----<EOM>-----" << std::endl << std::endl;;
    }
}

/**
 * @returns path to the file that user provided.
 */
std::string client::getFilePath() {
    std::string msg;
    std::cout << "Please enter file's full path: " << std::endl;
    std::getline(std::cin, msg);
    return msg;
}

/**
 * @param path a to the file
 * @returns the file in bytes form
 */
bytes client::loadFile(const std::string &path) {
    //check if file exist.
    if (!boost::filesystem::exists(path))
        throw FileNotFound();

    std::ifstream file(path, std::ios::binary);
    file.seekg(0, std::ios::end);

    //get file size (fpos)
    auto fileSize = file.tellg();

    //init. content vector with filesize
    bytes content((unsigned int) fileSize);

    // move ptr to the start pos.
    file.seekg(0);

    // read the content:
    file.read((char *) &content[0], fileSize);
    file.close();
    return content;
}

/**
 *
 * @param content
 * @returns the path as string
 */
std::string client::saveFile(const bytes &fileBytes) {
    boost::filesystem::path path(boost::filesystem::current_path());
    path.append("%TMP%");

    // create %TMP% directory if not exist.
    if (!boost::filesystem::exists(path))
        boost::filesystem::create_directory(path);

    // make a string of the full new file path and write the content to the file.
    path.append(generateRandomAlphaNum());
    boost::filesystem::ofstream fout(path, std::ios::out | std::ios::binary);
    fout.write((char *) &fileBytes[0], fileBytes.size() * sizeof(char));
    fout.close();

    return path.string();
}

/**
 * Get's uuid from me.info
 * @returns uuid of the this client.
 */
bytes client::getUUID() {
    std::string uname;
    std::string uuid;

    // if file doesnt exist init. uuid vector of size 16
    if (!boost::filesystem::exists(ME_INFO))
        return bytes(UID_SIZE);

    /**
     * try and read the uuid from me.info
     */
    try {
        std::ifstream myInfoFile(ME_INFO);

        // read the first line (username)
        std::getline(myInfoFile, uname);

        // read the second line (uuid)
        std::getline(myInfoFile, uuid);
        myInfoFile.close();
    }
        // if error occured during reading, catch it.
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }
    // uuid len. validation.
    if (uuid.size() != UUID_LENGTH) {
        std::cout << "getUUID: Incorrect UUID length. (!= 32)." << std::endl;
        exit(1);
    }
    bytes uuid_asBytes;
    boost::algorithm::unhex(uuid, std::back_inserter(uuid_asBytes));

    /* DEBUG
    string decoded_uuid;
    boost::algorithm::unhex(uuid, std::back_inserter(decoded_uuid));
    std::cout << decoded_uuid << std::endl;
    for (u_char c: uuid_asBytes) {
        std::cout << c << std::endl;
    }*/
    return uuid_asBytes;
}

/**
 * generates random name size of FILENAME_LENGTH
 * @returns alphanumeric num
 */
std::string client::generateRandomAlphaNum() {
    std::string ret;
    std::string chars(
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "1234567890");
    boost::random::random_device rng;
    boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1);
    for (int i = 0; i < FILENAME_LENGTH; ++i) {
        ret += chars[index_dist(rng)];
    }
    return ret;
}
