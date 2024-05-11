/**
 * This class is responsible for parsing the response header from
 * the server and act accordingly.
 *
 * Server response Header Format:
 * ------------------------------------------------------------------
 *  server_version - byte
 *  response_code - 2 bytes - the list of codes avail at constants.h
 *  payload_size - 4 bytes  - the size of the response payload.
 *  payload - variable.
 *  -----------------------------------------------------------------
 *
 * (I choose to represent the payload as vector of "bytes". (e.g unsigned char))
 */
#ifndef RESPONSE_HEADER
#define RESPONSE_HEADER
#include "constants.h"

class responses {
private:
    // response header from the server
    int server_ver;
    uint16_t response_code;
    int payload_size;
    bytes *payload; /// bytes vector

    /* sets the response code from the server.
     * because we're parsing almost everything as bytes,
     * */
    void setResponseCode(const unsigned char *buffer);

    // reads and sets the payload size from the given header
    void setPayloadSize(unsigned char *header);

public:
    // constructor/destructor to handle responses from the server.
    explicit responses(unsigned char *buffer);
    ~responses();

    // returns the size of the response payload from the server
    int getPayloadSIze() const;

    // returns the server version
    int getServerVersion() const;

    // returns the response code from the server.
    int getResponseCode() const;

    /* pushes "size" bytes into the vector payload.
     * we call this function multiple times because the chunk for
     * the incoming data from the server is limited to 1024 */
    void push(unsigned char *chunk, int size);

    // returns a pointer to this response payload
    const bytes &getPayload();
};

#endif