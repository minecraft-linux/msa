#pragma once

#ifdef __APPLE__
#include <CommonCrypto/CommonCryptor.h>
#else
#include <openssl/evp.h>
#endif

#ifdef __APPLE__
class CCCryptorRefAuto {

private:
    CCCryptorRef ctx;

public:
    CCCryptorRefAuto(CCCryptorRef ctx) : ctx(ctx) {
    }
    ~CCCryptorRefAuto() {
        CCCryptorRelease(ctx);
    }

    CCCryptorRef& obj() {
        return ctx;
    }

    operator CCCryptorRef() {
        return ctx;
    }

};
#else
class EvpCipherAuto {

private:
    EVP_CIPHER_CTX* ctx;

public:
    EvpCipherAuto(EVP_CIPHER_CTX* ctx) : ctx(ctx) {
    }
    ~EvpCipherAuto() {
        EVP_CIPHER_CTX_free(ctx);
    }

    operator EVP_CIPHER_CTX*() {
        return ctx;
    }

};
#endif