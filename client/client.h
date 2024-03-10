#ifndef CLIENT_HEADER
#define CLIENT_HEADER

#include <string>
#include "UsersList.h"
#include "constants.h"
#include "crypto_handler.h"
#include "Request.h"
#include "Response.h"
#include <boost/algorithm/hex.hpp>

using std::string;

class client {
private:
    /**
    * each client holds info about:
    * request, userlist,  keys and his username.
    * I choose to represent each one as byte vector(uchar).
    */
    Request *request;
    UsersList *usersList;
    crypto_handler *keys;
    string userName;


    // Prints out the menu with avaliable options for the client.
    void printMenu();

    // gets user selection code.
    int getUserSelection();

    // build the request vector based on client's selection.
    bool requestBuilder(int);

    // returns request opccode based on user selection.
    uint16_t getReqCode(int);

    //  Gets the desired username from the client.
    string getUserName();

    // saves username, uuid and privatekey into me.info
    bytes getMessage();


    // retrieves users list from given payload payload and saves it
    void generateUserList(const bytes &payload);

    // saves username and id to info file
    void saveUserInfo(const bytes &response);

    // pretty print the messages we got from the server.
    void printMessages(const bytes &);

    // returns path to the file that user provided.
    static string getFilePath();


    // receive a string to a path and returns a bytes with the file in this path
    static bytes loadFile(const string &);

    // saves a file in bytes form to disk. returns full path to this file as a string
    string saveFile(const bytes &);

    // generates random name
    static string generateRandomAlphaNum();
public:
    client();// init. all vectors and my key.
    ~client();// destroy all.

    // handle user input.
    bool io();

    // returns the Request bytes
    const bytes &getRequest();

    //handle responses and parse is accordingly
    void responseParser(Response *response);

    // gets client uuid from me.info
    static bytes getUUID();
};

#endif
