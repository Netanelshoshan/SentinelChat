/**
 * this class is responsible for all encryption and decryption of our chat program.
 */
#ifndef CRYPTO_HANDLER_HEADER
#define CRYPTO_HANDLER_HEADER

#include <cryptopp/osrng.h>

using CryptoPP::AutoSeededRandomPool;

#include <cryptopp/base64.h>
#include <cryptopp/files.h>
#include <cryptopp/modes.h>
#include <cryptopp/rsa.h>

using CryptoPP::RSA;

#include <cryptopp/filters.h>

using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::AuthenticatedEncryptionFilter;
using CryptoPP::AuthenticatedDecryptionFilter;
using CryptoPP::ByteQueue;

#include <cryptopp/files.h>

using CryptoPP::FileSource;

#include <cryptopp/aes.h>

using CryptoPP::AES;

#include <cryptopp/hex.h>

using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include <cryptopp/cryptlib.h>

using CryptoPP::BufferedTransformation;
using CryptoPP::AuthenticatedSymmetricCipher;

#include <cryptopp/gcm.h>

using CryptoPP::GCM;
using CryptoPP::GCM_TablesOption;

#include <cryptopp/secblock.h>

using CryptoPP::SecByteBlock;

#include <cryptopp/rsa.h>

using CryptoPP::RSAFunction;
using namespace CryptoPP;

#include <boost/filesystem.hpp>
#include <random>
#include "constants.h"
#include "Utils.h"
#include <algorithm>
#include "UsersList.h"
#include <sstream>

using std::string;

class crypto_handler {
private:
    RSA::PrivateKey privateKey;

    // loads the private key from me.info file.
    void loadPrivateKey();

    // returns random number generated key.
    char *keygen(char *key, size_t size);
public:
    //constructor that loads the private key if exist or init a new one.
    crypto_handler();

    // generate public key based on object's private key.
    bytes generatePublicKey();

    // sets the public key for a user from the response payload
    void setPublicKey(UsersList &usersList, const bytes &response);

    //loads the private key from me.info.
    void getPrivateKey();

    // Encode the private key with base64 and adds it into me.info.
    void savePrivateKey();

    // generates and returns a symmetric key.
    bytes generateSymmetricKey();

    //encrypts the symmetric keu with the public key.
    bytes encryptRSA(const bytes &publicKey, const bytes &symmetricKey);

    // Gets the cipher and decrypt it with the private key.
    bytes decryptRSA(const bytes &cipher);

    //Enctypts the message content with a symmetric key.
    bytes encryptAES(const bytes &symmetricKey, const bytes &plain);

    //Decrypt cipher (enccoded message) with symmetric key.
    bytes decryptAES(const bytes &symmetricKey, const bytes &cipher);
};


#endif