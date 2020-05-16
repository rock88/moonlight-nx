#include <stdio.h>
#include "Data.hpp"

extern "C" {
    #include "mkcert.h"
    #include "client.h"
}

#pragma once

#define CERTIFICATE_FILE_NAME "client.pem"
#define P12_FILE_NAME "client.p12"
#define KEY_FILE_NAME "key.pem"

class CryptoManager {
public:
    static bool certs_exists();
    static bool load_certs();
    static bool generate_certs();
    static Data SHA1_hash_data(Data data);
    static Data SHA256_hash_data(Data data);
    static Data create_AES_key_from_salt_SHA1(Data salted_pin);
    static Data create_AES_key_from_salt_SHA256(Data salted_pin);
    static Data aes_encrypt(Data data, Data key);
    static Data aes_decrypt(Data data, Data key);
    static Data pem_to_der(Data pem_cert_bytes);
    static bool verify_signature(Data data, Data signature, Data cert);
    static Data sign_data(Data data, Data key);
    static Data get_signature_from_cert(Data cert);
    static Data get_key_from_cert_key_pair(PCERT_KEY_PAIR cert_key_pair);
    static Data get_p12_from_cert_key_pair(PCERT_KEY_PAIR cert_key_pair);
    static Data get_cert_from_cert_key_pair(PCERT_KEY_PAIR cert_key_pair);
};
