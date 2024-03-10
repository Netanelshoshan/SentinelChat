#include "Utils.h"

/**
 * convert 4 bytes to int.
 * @param ptr
 * @returns integer
 */
int bytesToInt(const unsigned char *ptr) {
    int ret;
    ret = (unsigned char) ptr[3] << 24;
    ret += (unsigned char) ptr[2] << 16;
    ret += (unsigned char) ptr[1] << 8;
    ret += (unsigned char) ptr[0];
    return ret;
}