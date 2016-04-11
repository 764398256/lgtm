// g++ -std=c++11 lgtm_crypto.cpp lgtm_crypto_runner.cpp -o lgtm_crypto_runner -lcryptopp -lpthread

#include "lgtm_crypto.hpp"

//~Function Headers---------------------------------------------------------------------------------
void readFromFile(const string &fileName, SecByteBlock &input);
void writeToFile(const string &fileName, SecByteBlock &output);

//~Constants----------------------------------------------------------------------------------------
static const string LGTM_CRYPTO_PREFIX = ".lgtm-crypto-params-";
// Crypto parameters file names
static const string PUBLIC_KEY_FILE_NAME = LGTM_CRYPTO_PREFIX + "public-key";
static const string PRIVATE_KEY_FILE_NAME = LGTM_CRYPTO_PREFIX + "private-key";
static const string OTHER_PUBLIC_KEY_FILE_NAME = LGTM_CRYPTO_PREFIX + "other-public-key";
static const string SHARED_SECRET_FILE_NAME = LGTM_CRYPTO_PREFIX + "shared-secret";
static const string COMPUTED_KEY_FILE_NAME = LGTM_CRYPTO_PREFIX + "computed-key";
static const string VERIFICATION_MAC_FILE_NAME = LGTM_CRYPTO_PREFIX + "verification-mac";
static const string OTHER_VERIFICATION_MAC_FILE_NAME = LGTM_CRYPTO_PREFIX + "other-verification-mac";
static const string CURRENT_IV = LGTM_CRYPTO_PREFIX + "initialization-vector";

// Message file names
static const string FIRST_MESSAGE_FILE_NAME = LGTM_CRYPTO_PREFIX + "first-message";
static const string FIRST_MESSAGE_REPLY_FILE_NAME = LGTM_CRYPTO_PREFIX + "first-message-reply";

static const string SECOND_MESSAGE_FILE_NAME = LGTM_CRYPTO_PREFIX + "second-message";
static const string SECOND_MESSAGE_REPLY_FILE_NAME = LGTM_CRYPTO_PREFIX + "second-message-reply";

static const string THIRD_MESSAGE_FILE_NAME = LGTM_CRYPTO_PREFIX + "third-message";
static const string THIRD_MESSAGE_REPLY_FILE_NAME = LGTM_CRYPTO_PREFIX + "third-message-reply";

// Other LGTM file names
static const string FACIAL_RECOGNITION_FILE_NAME = ".lgtm-facial-recognition-params";
static const string ENCRYPTED_FACIAL_RECOGNITION_FILE_NAME = ".lgtm-facial-recognition-params--encrypted";
static const string RECEIVED_FACIAL_RECOGNITION_FILE_NAME = ".lgtm-received-facial-recognition-params";
static const string DECRYPTED_RECEIVED_FACIAL_RECOGNITION_FILE_NAME = ".lgtm-facial-recognition-params--decrypted";

//~Functions----------------------------------------------------------------------------------------
void firstMessage() {
    // Prepare Diffie-Hellman parameters
    SecByteBlock publicKey;
    SecByteBlock privateKey;
    generateDiffieHellmanParameters(publicKey, privateKey);

    // Write public key and private key to files
    writeToFile(PRIVATE_KEY_FILE_NAME, privateKey);
    writeToFile(PUBLIC_KEY_FILE_NAME, publicKey);
}

void replyToFirstMessage() {
    // Read in received Diffie-Hellman public key from file
    SecByteBlock otherPublicKey;
    readFromFile(OTHER_PUBLIC_KEY_FILE_NAME, otherPublicKey);

    // Prepare Diffie-Hellman parameters
    SecByteBlock publicKey;
    SecByteBlock privateKey;
    generateDiffieHellmanParameters(publicKey, privateKey);

    // Compute shared secret
    SecByteBlock sharedSecret;
    diffieHellmanSharedSecretAgreement(sharedSecret, otherPublicKey, privateKey);

    // Compute key from shared secret
    SecByteBlock key;
    generateSymmetricKeyFromSharedSecret(key, sharedSecret);

    // Write to file
    writeToFile(SHARED_SECRET_FILE_NAME, sharedSecret);
    writeToFile(COMPUTED_KEY_FILE_NAME, key);
}

void secondMessage() {
    // Read from file
    SecByteBlock privateKey;
    SecByteBlock otherPublicKey;
    readFromFile(PRIVATE_KEY_FILE_NAME, privateKey);
    readFromFile(OTHER_PUBLIC_KEY_FILE_NAME, otherPublicKey);

    // Compute shared secret
    SecByteBlock sharedSecret;
    diffieHellmanSharedSecretAgreement(sharedSecret, otherPublicKey, privateKey);

    // Compute key from shared secret
    SecByteBlock key;
    generateSymmetricKeyFromSharedSecret(key, sharedSecret);

    // Compute mac over all prior messages
    SecByteBlock mac;
    // TODO: 

    // Write to file
    writeToFile(SHARED_SECRET_FILE_NAME, sharedSecret);
    writeToFile(COMPUTED_KEY_FILE_NAME, key);
    writeToFile(VERIFICATION_MAC_FILE_NAME, mac);
}

void replyToSecondMessage() {
    // Read from file
    SecByteBlock key;
    SecByteBlock receivedMac;
    readFromFile(COMPUTED_KEY_FILE_NAME, key);
    readFromFile(OTHER_VERIFICATION_MAC_FILE_NAME, receivedMac);

    // Verify received Mac
    // TODO: 

    // Encrypt hash of all prior messages + this
    SecByteBlock mac;

    // Write to file
    writeToFile(VERIFICATION_MAC_FILE_NAME, mac);
}

void thirdMessage() {
    // Read session key from file
    SecByteBlock key;
    readFromFile(COMPUTED_KEY_FILE_NAME, key);

    // Read in the current initialization vector from file
    byte curIv[AES::BLOCKSIZE];
    // TODO: actually read it in

    // Verify received MAC
    // TODO:

    // Compute MAC of all prior messages + this
    // TODO: (must reconsider too....)

    // Encrypt (facial recognition params + MAC)
    encryptFile(FACIAL_RECOGNITION_FILE_NAME, "", ENCRYPTED_FACIAL_RECOGNITION_FILE_NAME, 
            key, curIv);
}

void thirdMessageReply() {
    // Read session key from file
    SecByteBlock key;
    readFromFile(COMPUTED_KEY_FILE_NAME, key);

    // Read in the current initialization vector from file
    byte curIv[AES::BLOCKSIZE];
    // TODO: actually read it in

    // Verify received MAC
    // TODO:

    // Compute MAC of all prior messages + this
    // TODO:

    // Encrypt (facial recognition params + MAC)
    encryptFile(FACIAL_RECOGNITION_FILE_NAME, "", ENCRYPTED_FACIAL_RECOGNITION_FILE_NAME, 
            key, curIv);
}

void thirdMessageDecryptReply() {
    // Read session key from file
    SecByteBlock key;
    readFromFile(COMPUTED_KEY_FILE_NAME, key);

    // Read in the current initialization vector from file
    byte curIv[AES::BLOCKSIZE];
    // TODO: actually read it in

    // Verify received MAC
    // TODO:

    decryptFile(RECEIVED_FACIAL_RECOGNITION_FILE_NAME, "", DECRYPTED_RECEIVED_FACIAL_RECOGNITION_FILE_NAME, 
        key, curIv);
}

/**
 * Reads into a SecByteBlock from a file specified by fileName.
 */
void readFromFile(const string &fileName, SecByteBlock &input) {
    ifstream inputStream(fileName, ios::in | ios::binary);
    // Get file length
    inputStream.seekg(0, inputStream.end);
    int fileLength = inputStream.tellg();
    inputStream.seekg(0, inputStream.beg);
    input.CleanNew(fileLength);
    inputStream.read((char*) input.BytePtr(), input.SizeInBytes());
    inputStream.close();
}

/**
 * Writes a SecByteBlock to a file specified by fileName.
 */
void writeToFile(const string &fileName, SecByteBlock &output) {
    ofstream outputStream(fileName, ios::in | ios::binary);
    outputStream.write((char*) output.BytePtr(), output.SizeInBytes());
    outputStream.close();
}

//~Main function------------------------------------------------------------------------------------
int main(int argc, char *argv[]) {

    if (argc != 2) {
        return 1;
    }

    if (strncmp(argv[0], "first-message", 13)) {
        firstMessage();
    } else if (strncmp(argv[0], "first-message-reply", 19)) {
        replyToFirstMessage();
    } else if (strncmp(argv[0], "second-message", 14)) {
        secondMessage();
    } else if (strncmp(argv[0], "second-message-reply", 20)) {
        replyToSecondMessage();
    } else if (strncmp(argv[0], "third-message", 13)) {
        thirdMessage();
    }
    // First message
    // Reply to first message
    // Second message
    // Reply to second message
    // Encrypt
    return 0;
}