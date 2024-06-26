#include "responses.h"
#include "utils.h"

/** sets the response code from header.
 * because response_code is an integer, we get the 4 bytes that we need
 * from the header and converts them to int.
 * @param header header
 * */
void responses::setResponseCode(const unsigned char *buffer) {
    response_code = buffer[1] & 0xFF;
    response_code += buffer[2] << 8;
}

/**
 * sets the payload size from header.
 * because payload_size is an integer, we get the 4 bytes that we need
 * from the header and converts them to int.
 * @param header header
 * */
void responses::setPayloadSize(unsigned char *header) {
    const unsigned char tmp[] = {header[3], header[4], header[5], header[6]};
    payload_size = bytesToInt(tmp);
}

/**
 * constructs the response and parse the header.
 * @param header the response header.
 */
responses::responses(unsigned char *buffer) {
    server_ver = buffer[0];
//    assert(server_ver != SERVER_VERSION);
    response_code = 0;
    payload_size = 0;
    setResponseCode(buffer);
    setPayloadSize(buffer);
    payload = new bytes();
}

/**
 * deletes allocated space for the payload.
 */
responses::~responses() {
    delete payload;
}

/**
 *
 * @returns the payload size
 */
int responses::getPayloadSIze() const {
    return payload_size;
}

/**
 *
 * @returns the server version.
 */
int responses::getServerVersion() const {
    return server_ver;
};

/**
 *
 * @returns the response code.
 */
int responses::getResponseCode() const {
    return response_code;
}

/**
 * push chunk of data to the payload
 * @param chunk chunk of daata
 * @param size the size of the chunk
 */
void responses::push(unsigned char *chunk, int size) {
    for (int i = 0; i < size; i++)
        payload->push_back(chunk[i]);
}

/**
 *
 * @returns the payload
 */
const bytes &responses::getPayload() {
    return *payload;
}
