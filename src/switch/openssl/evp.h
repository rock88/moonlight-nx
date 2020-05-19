#include <stdint.h>

#define EVP_CTRL_GCM_SET_IVLEN 0x9
#define EVP_CTRL_GCM_GET_TAG 0x10

typedef int EVP_CIPHER;
typedef int ENGINE;

struct MBED_CIPHER_CTX;
typedef struct MBED_CIPHER_CTX EVP_CIPHER_CTX;

#ifdef __cplusplus
extern "C" {
#endif

const EVP_CIPHER *EVP_aes_128_gcm(void);
const EVP_CIPHER *EVP_aes_128_cbc(void);

void EVP_CIPHER_CTX_init(EVP_CIPHER_CTX *ctx);
int EVP_CIPHER_CTX_cleanup(EVP_CIPHER_CTX *ctx);
EVP_CIPHER_CTX *EVP_CIPHER_CTX_new(void);
void EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *ctx);
int EVP_CIPHER_CTX_ctrl(EVP_CIPHER_CTX *ctx, int type, int arg, void *ptr);

int EVP_EncryptInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher, ENGINE *impl, const unsigned char *key, const unsigned char *iv);
int EVP_EncryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl);
int EVP_EncryptFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl);

#ifdef __cplusplus
}
#endif
