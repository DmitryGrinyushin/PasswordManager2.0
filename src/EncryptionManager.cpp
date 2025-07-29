#include "EncryptionManager.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <stdexcept>
#include <cstring>
#include <iostream> // for debug output

// Helper: base64 encode
std::string base64Encode(const std::vector<unsigned char>& data) {
    BIO* bio = BIO_new(BIO_s_mem());
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // no newlines
    bio = BIO_push(b64, bio);

    BIO_write(bio, data.data(), static_cast<int>(data.size()));
    BIO_flush(bio);

    BUF_MEM* bufferPtr;
    BIO_get_mem_ptr(bio, &bufferPtr);
    std::string result(bufferPtr->data, bufferPtr->length);

    BIO_free_all(bio);
    return result;
}

// Helper: base64 decode
std::vector<unsigned char> base64Decode(const std::string& input) {
    BIO* bio = BIO_new_mem_buf(input.data(), static_cast<int>(input.size()));
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);

    std::vector<unsigned char> buffer(input.size());
    int length = BIO_read(bio, buffer.data(), static_cast<int>(input.size()));
    BIO_free_all(bio);

    if (length <= 0)
        throw std::runtime_error("base64Decode failed: invalid input or read error");

    buffer.resize(length);
    return buffer;
}

std::vector<unsigned char> EncryptionManager::generateRandomBytes(size_t length) {
    std::vector<unsigned char> buffer(length);
    if (RAND_bytes(buffer.data(), static_cast<int>(length)) != 1) {
        throw std::runtime_error("Failed to generate random bytes");
    }
    return buffer;
}

std::vector<unsigned char> EncryptionManager::encrypt(
    const std::string& plaintext,
    const std::vector<unsigned char>& key,
    std::vector<unsigned char>& outIV,
    std::vector<unsigned char>& outTag
) {
    const size_t ivLength = 12; // standard IV length for GCM
    outIV = generateRandomBytes(ivLength);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create encryption context");

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1)
        throw std::runtime_error("Failed to initialize AES-GCM encryption");

    if (EVP_EncryptInit_ex(ctx, nullptr, nullptr, key.data(), outIV.data()) != 1)
        throw std::runtime_error("Failed to set encryption key and IV");

    std::vector<unsigned char> ciphertext(plaintext.size());
    int len = 0;

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
                          reinterpret_cast<const unsigned char*>(plaintext.data()),
                          static_cast<int>(plaintext.size())) != 1)
        throw std::runtime_error("Encryption failed during update");

    int ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1)
        throw std::runtime_error("Encryption failed during finalization");

    ciphertext_len += len;
    ciphertext.resize(ciphertext_len); // Trim to actual length

    outTag.resize(16); // 128-bit tag
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, outTag.data()) != 1)
        throw std::runtime_error("Failed to retrieve GCM tag");

    EVP_CIPHER_CTX_free(ctx);

    std::cout << "[ENCRYPT DEBUG] iv=" << base64Encode(outIV)
              << " ciphertext=" << base64Encode(ciphertext)
              << " tag=" << base64Encode(outTag) << "\n";

    return ciphertext;
}

std::string EncryptionManager::decrypt(
    const std::vector<unsigned char>& ciphertext,
    const std::vector<unsigned char>& key,
    const std::vector<unsigned char>& iv,
    const std::vector<unsigned char>& tag
) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create decryption context");

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1)
        throw std::runtime_error("Failed to initialize AES-GCM decryption");

    if (EVP_DecryptInit_ex(ctx, nullptr, nullptr, key.data(), iv.data()) != 1)
        throw std::runtime_error("Failed to set decryption key and IV");

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, const_cast<unsigned char*>(tag.data())) != 1)
        throw std::runtime_error("Failed to set GCM tag");

    std::vector<unsigned char> plaintext(ciphertext.size());
    int len = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), static_cast<int>(ciphertext.size())) != 1)
        throw std::runtime_error("Decryption failed during update");

    int plaintext_len = len;

    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1)
        throw std::runtime_error("Decryption failed: authentication check failed");

    plaintext_len += len;
    plaintext.resize(plaintext_len);

    EVP_CIPHER_CTX_free(ctx);
    return std::string(plaintext.begin(), plaintext.end());
}

std::string EncryptionManager::encryptField(const std::string& plaintext, const std::vector<unsigned char>& key) {
    std::vector<unsigned char> iv, tag;
    std::vector<unsigned char> ciphertext = encrypt(plaintext, key, iv, tag);

    return base64Encode(iv) + "|" + base64Encode(ciphertext) + "|" + base64Encode(tag);
}

std::string EncryptionManager::decryptField(const std::string& encrypted, const std::vector<unsigned char>& key) {
    size_t pos1 = encrypted.find('|');
    size_t pos2 = encrypted.find('|', pos1 + 1);

    if (pos1 == std::string::npos || pos2 == std::string::npos)
        throw std::runtime_error("Invalid encrypted format");

    std::string ivB64 = encrypted.substr(0, pos1);
    std::string ciphertextB64 = encrypted.substr(pos1 + 1, pos2 - pos1 - 1);
    std::string tagB64 = encrypted.substr(pos2 + 1);

    std::vector<unsigned char> iv = base64Decode(ivB64);
    std::vector<unsigned char> ciphertext = base64Decode(ciphertextB64);
    std::vector<unsigned char> tag = base64Decode(tagB64);

    return decrypt(ciphertext, key, iv, tag);
}