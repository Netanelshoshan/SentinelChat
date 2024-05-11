/**
 * **** The encryption configuration used here is just for demonstration purposes only.****
 * Using CBC mode in AES is not recommended and you should use GCM mode
 * that provides AEAD.
 * Another thing is the iv being always 0.
 */

#include "crypto_handler.h"
#include "exceptions.h"

/**
 * constructor that loads the private key if exist or init a new one.
 * @param infoFileExists bool flag that ge
 */
crypto_handler::crypto_handler() {
    loadPrivateKey();
}

// loads the private key from me.info.
void crypto_handler::getPrivateKey() {
    loadPrivateKey();
}

/**
 * Loads private key from me.info file.
 */
void crypto_handler::loadPrivateKey() {
    if (boost::filesystem::exists(ME_INFO)) {
        int pos;
        //determine the index for the private key in file
        try {
            string username, uuid;
            std::ifstream f(ME_INFO);
            if (!f)
                throw FileNotFound();
            getline(f, username);
            getline(f, uuid);
            pos = f.tellg();
            f.close();
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
            exit(1);
        }

        //pump the private key into the bytequeue and decode it.
        try {
            AutoSeededRandomPool rng;
            ByteQueue bytes;
            FileSource file(ME_INFO, false);
            file.Pump(pos);
            file.Attach(new CryptoPP::Base64Decoder);
            file.PumpAll();
            file.TransferTo(bytes);
            bytes.MessageEnd();
            privateKey.Load(bytes);
        }
        catch (std::exception &e) {
            std::cout << e.what() << std::endl;
            exit(1);
        }
    }
    else {
        AutoSeededRandomPool rng;
        privateKey.Initialize(rng, PUB_KEY_BITS);
    }
}

/**
 * Encrypts the public key with the symmetric key.
 * @param pubKey
 * @param symmetricKey
 * @return the cipher
 */
bytes crypto_handler::encryptRSA(const bytes &pubKey, const bytes &symmetricKey) {
    std::string cipher, pkey(pubKey.begin(), pubKey.end()), skey(symmetricKey.begin(), symmetricKey.end());
    AutoSeededRandomPool rng;
    StringSource ss(pkey, true);
    RSA::PublicKey publicKey;
    publicKey.Load(ss);
    CryptoPP::RSAES_OAEP_SHA_Encryptor e(publicKey);
    StringSource ss1(skey, true,
                     new CryptoPP::PK_EncryptorFilter(rng, e,
                                                      new StringSink(cipher)));
    bytes c(cipher.begin(), cipher.end());
    return c;
}


/**
 * Gets the cipher and decrypt it with the private key.
 * @param cipher
 * @return the
 */
bytes crypto_handler::decryptRSA(const bytes &cipher) {
    CryptoPP::AutoSeededRandomPool rng;
    std::string decoded;
    std::string c1(cipher.begin(), cipher.end());
    CryptoPP::RSAES_OAEP_SHA_Decryptor d(privateKey);
    StringSource ss2(c1, true,
                     new CryptoPP::PK_DecryptorFilter(rng, d,
                                                      new StringSink(decoded)));
    bytes dec(decoded.begin(), decoded.end());
    return dec;

}


/**
 *
 * @param key the byte array
 * @param size the size of key
 * @return random number generated key.
 */
char *crypto_handler::keygen(char *key, size_t size) {
    for (size_t i = 0; i < size; i += sizeof(unsigned int))
        rand_r(reinterpret_cast<unsigned int *>(&key[i]));
    return key;
}

/**
 * @return a symmetric key
 */
bytes crypto_handler::generateSymmetricKey() {
    CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], iv[CryptoPP::AES::BLOCKSIZE];
    memset(key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
    memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE);
    keygen(reinterpret_cast<char *>(key), CryptoPP::AES::DEFAULT_KEYLENGTH);
    bytes res;
    for (auto c: key)
        res.push_back(c);
    return res;
}

/**
 * Enctypts the message content with a symmetric key.
 * @param symmetricKey
 * @param plain
 * @return encrypted msg.
 */
bytes crypto_handler::encryptAES(const bytes &symmetricKey, const bytes &plain) {
    string cipher;
    CryptoPP::byte key[AES::DEFAULT_KEYLENGTH];
    CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
    memset(key, 0x00, AES::DEFAULT_KEYLENGTH);
    memset(iv, 0x00, AES::BLOCKSIZE);
    for (int i = 0; i < AES::DEFAULT_KEYLENGTH; i++)
        key[i] = symmetricKey.at(i);
    AES::Encryption aesEncryption(key, AES::DEFAULT_KEYLENGTH);
    CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);
    StreamTransformationFilter encryptor(cbcEncryption, new CryptoPP::StringSink(cipher));
    encryptor.Put(&(plain[0]), plain.size());
    encryptor.MessageEnd();
    bytes b = bytes(cipher.begin(), cipher.end());
    return b;
}

/**
 * Decrypt cipher (enccoded message) with symmetric key
 * @param symmetricKey
 * @param cipher
 * @return decrypted message.
 */
bytes crypto_handler::decryptAES(const bytes &symmetricKey, const bytes &cipher) {
    std::string decoded;
    CryptoPP::byte key[AES::DEFAULT_KEYLENGTH];
    CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
    memset(key, 0x00, AES::DEFAULT_KEYLENGTH);
    memset(iv, 0x00, AES::BLOCKSIZE);
    for (int i = 0; i < AES::DEFAULT_KEYLENGTH; i++)
        key[i] = symmetricKey.at(i);
    CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);
    CryptoPP::StreamTransformationFilter decryptor(cbcDecryption, new CryptoPP::StringSink(decoded));
    decryptor.Put(&(cipher[0]), cipher.size());
    decryptor.MessageEnd();
    bytes d(decoded.begin(), decoded.end());
    return d;
}

/**
 * @return public key based on object's private key.
 */
bytes crypto_handler::generatePublicKey() {
    string key;
    RSAFunction publicKey(privateKey);
    StringSink ss(key);
    publicKey.Save(ss);
    bytes b(key.begin(), key.end());
    return b;
}

/**
 * set a public key for uuid in th payload.
 * @param usersList the list of current registered users.
 * @param payload the response payload which were going to parse.
 */
void crypto_handler::setPublicKey(users &usersList, const bytes &payload) {
    bytes uid;
    for (int i = 0; i < UID_SIZE; i++)
        uid.push_back(payload.at(i));
    bytes publicKey(payload.begin() + UID_SIZE, payload.end());
    std::cout << std::endl;
    usersList.setPublicKey(uid, publicKey);
}

/**
 * Encode the private key with base64 and DER and add it into me.info.
 */
void crypto_handler::savePrivateKey() {
    std::fstream infoFile(ME_INFO, std::fstream::out | std::fstream::app);
    Base64Encoder encoder(new FileSink(infoFile));
    privateKey.DEREncode(encoder);
    encoder.MessageEnd();
}

