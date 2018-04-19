#include <msa/network/crypto_utils.h>
#include <random>
#include <cstring>
#include <base64.h>
#ifdef __APPLE__
#include <CommonCrypto/CommonCrypto.h>
#include <CommonCrypto/CommonHMAC.h>
#include <CommonCrypto/CommonCryptor.h>
#else
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <netinet/in.h>

#endif

using namespace msa::network;

std::string CryptoUtils::createNonce(size_t size) {
    std::string ret;
    ret.resize(size);
    std::random_device rd;
    unsigned int* data = (unsigned int*) &ret[0];
    for (int i = 0; i < size / sizeof(unsigned int); i++)
        data[i] = rd();
    return ret;
}

std::string CryptoUtils::sign(std::string const& data, std::string const& binarySecret, std::string const& keyUsage,
                              std::string const& nonce) {
    std::string signatureKey = generateSharedKey(32, binarySecret, keyUsage, nonce);
#ifdef __APPLE__
    unsigned char signatureBuf[CC_SHA256_DIGEST_LENGTH];
    CCHmac(kCCHmacAlgSHA256, signatureKey.data(), signatureKey.length(), (unsigned char*) data.data(), data.size(), signatureBuf);
    return Base64::encode(std::string((char*) signatureBuf, sizeof(signatureBuf)));
#else
    unsigned char signatureBuf[EVP_MAX_MD_SIZE];
    unsigned int signatureSize = 0;
    HMAC(EVP_sha256(), signatureKey.data(), signatureKey.length(), (unsigned char*) data.data(), data.size(), signatureBuf, &signatureSize);
    return Base64::encode(std::string((char*) signatureBuf, signatureSize));
#endif
}

std::string CryptoUtils::generateSharedKey(size_t keyLength, std::string const& sessionKey, std::string const& keyUsage,
                                           std::string const& nonce) {
    std::string ret;
    ret.reserve((size_t) keyLength);
    unsigned char* buf = new unsigned char[sizeof(int) + keyUsage.length() + sizeof(char) + nonce.length() + sizeof(int)];
#ifdef __APPLE__
    unsigned char resultBuf[CC_SHA256_DIGEST_LENGTH];
#else
    unsigned char resultBuf[EVP_MAX_MD_SIZE];
    unsigned int resultSize = 0;
#endif
    size_t off = 0;
    off += sizeof(int);
    memcpy(&buf[off], keyUsage.data(), keyUsage.length()); off += keyUsage.length();
    buf[off] = 0; off++;
    memcpy(&buf[off], nonce.data(), nonce.length()); off += nonce.length();
    ((int&) buf[off]) = htonl((uint32_t) (keyLength * 8)); off += sizeof(int);
    size_t i = 1;
    while (ret.length() < keyLength) {
        ((int&) buf[0]) = htonl((uint32_t) i++);
#ifdef __APPLE__
        CCHmac(kCCHmacAlgSHA256, sessionKey.data(), sessionKey.length(), buf, off, resultBuf);
        ret.append((char*) resultBuf, std::min<size_t>(CC_SHA256_DIGEST_LENGTH, keyLength - ret.size()));
#else
        HMAC(EVP_sha256(), sessionKey.data(), (int) sessionKey.length(), buf, off, resultBuf, &resultSize);
        ret.append((char*) resultBuf, std::min<size_t>(resultSize, keyLength - ret.size()));
#endif
    }
    delete[] buf;
    return ret;
}

std::string CryptoUtils::sha256(std::string const& data) {
    std::string hash;
#ifdef __APPLE__
    hash.resize(CC_SHA256_DIGEST_LENGTH);
    CC_SHA256(text.data(), text.length(), (unsigned char*) &hash[0]);
#else
    unsigned int hashLen = SHA256_DIGEST_LENGTH;
    hash.resize(hashLen);
    EVP_Digest((unsigned char*) data.data(), data.length(),
               (unsigned char*) &hash[0], &hashLen,
               EVP_sha256(), nullptr);
    if (hashLen != SHA256_DIGEST_LENGTH)
        hash.resize(hashLen);
#endif
    return hash;
}