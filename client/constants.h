#ifndef CONSTANTS_HEADER
#define CONSTANTS_HEADER

#include <vector>
#include <cryptopp/aes.h>

typedef std::vector<unsigned char> bytes;

#define SERVER_INFO_FILE "server.info"
#define ME_INFO "me.info"

static const int CLIENT_VERSION = 2;
static const int SERVER_VERSION = 2;

// user selection input
enum selection {
    REG = 110,
    GET_USERLIST = 120,
    GET_PUBLIC_KEY = 130,
    GET_PENDING_MSGS = 140,
    SEND_MSG = 150,
    GET_SYMKEY = 151,
    SEND_SYMKEY = 152,
    SEND_FILE = 153,
    EXIT = 0,
    INVALID_INPUT = -1
};

// Request codes
static const uint16_t REG_REQ = 1100;
static const uint16_t GET_USER_LIST_REQ = 1101;
static const uint16_t GET_PUBKEY_REQ = 1102;
static const uint16_t SEND_MSG_REQ = 1103;
static const uint16_t GET_MSG_REQ = 1104;

// msg type codes
static const unsigned char MSG_TYPE_GET_SYM_KEY = 1;
static const unsigned char MSG_TYPE_SEND_SYM_KEY = 2;
static const unsigned char MSG_TYPE_SEND_MSG = 3;
static const unsigned char MSG_TYPE_SEND_FILE = 4;

// response codes
static const uint16_t REG_SUCC = 2100;
static const uint16_t GET_USER_LIST_SUCC = 2101;
static const uint16_t GET_PUB_KEY_SUCC = 2102;
static const uint16_t SEND_MSG_SUCC = 2103;
static const uint16_t GET_MSG_SUCC = 2104;
static const uint16_t INTERNAL_ERROR = 9000;

// byte sizes of fields from the protocol
static const int MAX_USERNAME_LEN = 255;
static const int UID_SIZE = 16;
static const int UUID_LENGTH = 32;
static const int RESPONSE_HEADER_SIZE = 7;

// encryption keys constants
static const int PUB_KEY_SIZE = 160;
static const int PUB_KEY_BITS = 1024;
static const int FILENAME_LENGTH = 8;
static const int BUFFER_SIZE = 1024;


#endif