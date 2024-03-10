/**
 * This class represent a Request for the client.
 * The client have several Request options.
 * Available options:
 * 1100 - register.
 * 1101 - get the list of users.
 * 1102 - get the public key of specific user.
 * 1103 - send a message/file to another user.
 *
 * There are 4 types of messages:
 * 1 - Request symmetric key.
 *     ( empty message. ContentSize = 0)
 * 2 - send my symmetric key.
 *     ( message content containing symmetric key that's encrypted
 *       by my friend's public key.)
 * 3 - send text message.
 *      ( message content containing text and encrypted a symmetric key.)
 * 4 - send file.
 *      ( message content containing encrypted file by a symmetric key)
 */
#ifndef REQUEST_HEADER
#define REQUEST_HEADER

#include "constants.h"

class Request {
private:
    bytes *request; // the request vector which will hold the header and the payload.
    uint16_t request_code; //  the request code.

public:
    Request();
    ~Request();

    // clears the Request and resets Request code.
    void clearRequest();

    // returns the Request code.
    uint16_t getRequestCode() const;

    // returns the Request.
    const bytes &getRequest();

    // push an integer to the Request vector as little endian integer
    void pushInt(int);

    // Builds the request header with given uuid and request code.
    void buildHeader(const bytes &uuid, uint16_t rcode);

    void getPendingMessages();
    // Builds the register request.
    void registerRequest(const bytes &userName, const bytes &pubKey);

    // Builds a request for list of registered users.
    void getUserListRequest();

    // Build a request for getting the public key of specified user.
    void getPublicKeyRequest(const bytes &uuid);

    // Send a message to a specific client.
    void sendMessageRequest(int payloadSize, const bytes &dstUser, unsigned char messageType, int messageSize,
                            const bytes &encoded);
};

#endif