#include <openssl/rand.h>
#include <openssl/evp.h>
#include <mbedtls/gcm.h>
#include <cstdlib>
#include "Data.hpp"

struct MBED_CIPHER_CTX {
    mbedtls_gcm_context ctx;
    int iv_len;
    const unsigned char *iv;
    unsigned char* tag;
};

const EVP_CIPHER *EVP_aes_128_gcm(void) {
    return NULL;
}

const EVP_CIPHER *EVP_aes_128_cbc(void) {
    return NULL;
}

void EVP_CIPHER_CTX_init(EVP_CIPHER_CTX *ctx) {
    if (ctx) {
        mbedtls_gcm_init(&ctx->ctx);
        ctx->iv_len = 0;
        ctx->iv = NULL;
        ctx->tag = NULL;
    }
}

int EVP_CIPHER_CTX_cleanup(EVP_CIPHER_CTX *ctx) {
    if (ctx) {
        mbedtls_gcm_free(&ctx->ctx);
        
        if (ctx->tag) {
            free(ctx->tag);
            ctx->tag = NULL;
        }
    }
    return 1;
}

EVP_CIPHER_CTX *EVP_CIPHER_CTX_new(void) {
    EVP_CIPHER_CTX* ctx = (EVP_CIPHER_CTX *)malloc(sizeof(EVP_CIPHER_CTX));
    EVP_CIPHER_CTX_init(ctx);
    return ctx;
}

void EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *ctx) {
    if (ctx && ctx->tag) {
        free(ctx->tag);
    }
}

int EVP_CIPHER_CTX_ctrl(EVP_CIPHER_CTX *ctx, int type, int arg, void *ptr) {
    if (ctx && type == EVP_CTRL_GCM_SET_IVLEN) {
        ctx->iv_len = arg;
    } else if (ctx && type == EVP_CTRL_GCM_GET_TAG) {
        unsigned char *tag = (unsigned char*)ptr;
        
        for (int i = 0; i < arg; i++) {
            tag[i] = ctx->tag[i];
        }
    }
    return 1;
}

int EVP_EncryptInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher, ENGINE *impl, const unsigned char *key, const unsigned char *iv) {
    if (ctx && key != NULL && iv != NULL) {
        mbedtls_gcm_setkey(&ctx->ctx, MBEDTLS_CIPHER_ID_AES, key, ctx->iv_len * 8);
        mbedtls_gcm_starts(&ctx->ctx, MBEDTLS_GCM_ENCRYPT, iv, ctx->iv_len, NULL, 0);
        ctx->iv = iv;
    }
    return 1;
}

int EVP_EncryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl) {
    if (ctx) {
        ctx->tag = (unsigned char*)malloc(ctx->iv_len);
        
        mbedtls_gcm_crypt_and_tag(&ctx->ctx, MBEDTLS_GCM_ENCRYPT, inl, ctx->iv, ctx->iv_len, NULL, 0, in, out, ctx->iv_len, ctx->tag);
        *outl = inl;
    }
    return 1;
}

int EVP_EncryptFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl) {
    return 1;
}

// TODO: This is correct?
int EVP_DecryptInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher, ENGINE *impl, const unsigned char *key, const unsigned char *iv) {
    if (ctx && key != NULL && iv != NULL) {
        mbedtls_gcm_setkey(&ctx->ctx, MBEDTLS_CIPHER_ID_AES, key, ctx->iv_len * 8);
        mbedtls_gcm_starts(&ctx->ctx, MBEDTLS_GCM_DECRYPT, iv, ctx->iv_len, NULL, 0);
        ctx->iv = iv;
    }
    return 1;
}

int EVP_DecryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl) {
    if (ctx) {
        ctx->tag = (unsigned char*)malloc(ctx->iv_len);
        
        mbedtls_gcm_crypt_and_tag(&ctx->ctx, MBEDTLS_GCM_DECRYPT, inl, ctx->iv, ctx->iv_len, NULL, 0, in, out, ctx->iv_len, ctx->tag);
        *outl = inl;
    }
    return 1;
}

int EVP_DecryptFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *outm, int *outl) {
    return 1;
}

int RAND_bytes(unsigned char *buf, int num) {
    Data rand = Data::random_bytes(num);
    for (int i = 0; i < num; i++) {
        buf[i] = rand.bytes()[i];
    }
    return 1;
}
