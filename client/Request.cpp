
#include "Request.h"

// initializes the Request vector and the Request code.
Request::Request() {
    request = new bytes();
    request_code = 0;
}

Request::~Request() {
    delete request;
}

/**
 * This function clears the request
 */
void Request::clearRequest() {
    request->clear();
    request_code = 0;
}

/**
 * @return request_code
 */
uint16_t Request::getRequestCode() const {
    return request_code;
}


/**
 * pushes our integer as little endian bytes by converting the 4 bytes
 * to uchar and pushing each one into the payload.
 * @param num the number we want to push into out payload.
 */
void Request::pushInt(int num) {
    unsigned int mask = 0x000000FF;
    for (int i = 0; i < sizeof(int); i++) {
        unsigned char tmp = (num & mask) >> (i * 8);
        request->push_back(tmp);
        mask <<= 8;
    }
}


/**
 * Builds the request header with given uuid and request code.
 * This function will always run first.
 * It constructs the first 3 fields of the header.
 * uuid - 16bytes | client version - 1byte | request code - 2bytes
 * @param uuid unique user id
 * @param rcode request code
 */
void Request::buildHeader(const bytes &uuid, uint16_t rcode) {
    clearRequest();
    request->insert(request->end(), uuid.begin(), uuid.end());
    request->push_back(CLIENT_VERSION);
    request->push_back(rcode & 0xFF);
    request->push_back(rcode >> 8);
    request_code = rcode;
}


/**
 * Builds the register request. Gets the username and the public key and pushes the
 * payload_size and the payload into the request vector.
 * @param userName gets the username
 * @param pubKey the public key for the user that we want to register
 */
void Request::registerRequest(const bytes &userName, const bytes &pubKey) {
    pushInt(MAX_USERNAME_LEN + PUB_KEY_SIZE);
    request->insert(request->end(), userName.begin(), userName.end());
    int nameSize = (int) userName.size();
    while (nameSize++ < 255)
        request->push_back('\0');
    bytes publicKey(pubKey.begin(), pubKey.end());
    request->insert(request->end(), publicKey.begin(), publicKey.end());
}

/**
 * if user is requesting the list of users, the payload field will be 0.
 */
void Request::getUserListRequest() {
    pushInt(0);
}

void Request::getPendingMessages(){
    pushInt(0);
}

/**
 * get public key of specified uuid
 * @param uuid
 */
void Request::getPublicKeyRequest(const bytes &uuid) {
    request->insert(request->end(), uuid.begin(), uuid.end());
}


/**
 * send a message to a specific client.
 * @param payloadSize the size of the payload.
 * @param dstUser the destination user.
 * @param messageType the type of message (1-4).
 * @param messageSize the size of the message.
 * @param encoded the encoded content of the message.
 */
void Request::sendMessageRequest(int payloadSize, const bytes &dstUser, unsigned char messageType, int messageSize,
                                 const bytes &encoded) {
    pushInt(payloadSize);
    request->insert(request->end(), dstUser.begin(), dstUser.end());
    request->push_back(messageType);
    pushInt(messageSize);
    bytes encodedMessage(encoded.begin(), encoded.end());
    request->insert(request->end(), encodedMessage.begin(), encodedMessage.end());
}

/**
 * @return a ptr to the request vector.
 */
const bytes &Request::getRequest() {
    return *request;
}
