#include "CryptoManager.hpp"
#include "Settings.hpp"
#include <string.h>
#include <cstdlib>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

#include "Log.h"

static const int SHA1_HASH_LENGTH = 20;
static const int SHA256_HASH_LENGTH = 32;

static Data* m_cert = nullptr;
static Data* m_p12 = nullptr;
static Data* m_key = nullptr;

bool CryptoManager::certs_exists() {
    if (load_certs()) {
        return true;
    }
    return false;
}

bool CryptoManager::load_certs() {
    if (m_key == nullptr || m_cert == nullptr || m_p12 == nullptr) {
        auto key_dir = Settings::settings()->working_dir() + "/key/";
        mkdirtree(key_dir.c_str());
        
        Data cert = Data::read_from_file(key_dir + CERTIFICATE_FILE_NAME);
        Data p12 = Data::read_from_file(key_dir + P12_FILE_NAME);
        Data key = Data::read_from_file(key_dir + KEY_FILE_NAME);
        
        if (!cert.is_empty() && !p12.is_empty() && !key.is_empty()) {
            m_cert = &cert;
            m_p12 = &p12;
            m_key = &key;
            return true;
        }
        
        return false;
    }
    return true;
}

bool CryptoManager::generate_certs() {
    auto cert_key_pair = mkcert_generate();
    
    Data cert = get_cert_from_cert_key_pair(&cert_key_pair);
    Data p12 = get_p12_from_cert_key_pair(&cert_key_pair);
    Data key = get_key_from_cert_key_pair(&cert_key_pair);
    
    auto key_dir = Settings::settings()->working_dir() + "/key/";
    mkdirtree(key_dir.c_str());
    
    if (!cert.is_empty() && !p12.is_empty() && !key.is_empty()) {
        cert.write_to_file(key_dir + CERTIFICATE_FILE_NAME);
        p12.write_to_file(key_dir + P12_FILE_NAME);
        key.write_to_file(key_dir + KEY_FILE_NAME);
        m_cert = &cert;
        m_p12 = &p12;
        m_key = &key;
        return true;
    }
    return false;
}

Data CryptoManager::SHA1_hash_data(Data data) {
    char sha1[SHA1_HASH_LENGTH];
    SHA1((unsigned char*)data.bytes(), data.size(), (unsigned char*)sha1);
    return Data(sha1, sizeof(sha1));
}

Data CryptoManager::SHA256_hash_data(Data data) {
    char sha256[SHA256_HASH_LENGTH];
    SHA256((unsigned char*)data.bytes(), data.size(), (unsigned char*)sha256);
    return Data(sha256, sizeof(sha256));
}

Data CryptoManager::create_AES_key_from_salt_SHA1(Data salted_pin) {
    return SHA1_hash_data(salted_pin).subdata(16);
}

Data CryptoManager::create_AES_key_from_salt_SHA256(Data salted_pin) {
    return SHA256_hash_data(salted_pin).subdata(16);
}

static int get_encrypt_size(Data data) {
    // the size is the length of the data ceiling to the nearest 16 bytes
    return (((int)data.size() + 15) / 16) * 16;
}

Data CryptoManager::aes_encrypt(Data data, Data key) {
    AES_KEY aes_key;
    AES_set_encrypt_key((unsigned char*)key.bytes(), 128, &aes_key);
    int size = get_encrypt_size(data);
    unsigned char* buffer = (unsigned char*)malloc(size);
    unsigned char* block_rounded_buffer = (unsigned char*)calloc(1, size);
    memcpy(block_rounded_buffer, data.bytes(), data.size());
    
    // AES_encrypt only encrypts the first 16 bytes so iterate the entire buffer
    int block_offset = 0;
    while (block_offset < size) {
        AES_encrypt(block_rounded_buffer + block_offset, buffer + block_offset, &aes_key);
        block_offset += 16;
    }
    
    Data encrypted_data = Data((char*)buffer, size);
    free(buffer);
    free(block_rounded_buffer);
    return encrypted_data;
}

Data CryptoManager::aes_decrypt(Data data, Data key) {
    AES_KEY aes_key;
    AES_set_decrypt_key((unsigned char*)key.bytes(), 128, &aes_key);
    unsigned char* buffer = (unsigned char*)malloc(data.size());
    
    // AES_decrypt only decrypts the first 16 bytes so iterate the entire buffer
    int block_offset = 0;
    while (block_offset < data.size()) {
        AES_decrypt((unsigned char*)data.bytes() + block_offset, buffer + block_offset, &aes_key);
        block_offset += 16;
    }
    
    Data decrypted_data = Data(buffer, data.size());
    free(buffer);
    return decrypted_data;
}

Data CryptoManager::pem_to_der(Data pem_cert_bytes) {
    X509* x509;
    
    BIO* bio = BIO_new_mem_buf((unsigned char*)pem_cert_bytes.bytes(), pem_cert_bytes.size());
    x509 = PEM_read_bio_X509(bio, NULL, NULL, NULL);
    BIO_free(bio);
    
    bio = BIO_new(BIO_s_mem());
    i2d_X509_bio(bio, x509);
    
    BUF_MEM* mem;
    BIO_get_mem_ptr(bio, &mem);
    
    Data ret = Data(mem->data, mem->length);
    BIO_free(bio);
    return ret;
}

bool CryptoManager::verify_signature(Data data, Data signature, Data cert) {
    X509* x509;
    BIO* bio = BIO_new_mem_buf(cert.bytes(), cert.size());
    x509 = PEM_read_bio_X509(bio, NULL, NULL, NULL);
    
    BIO_free(bio);
    
    if (!x509) {
        LOG("Unable to parse certificate in memory...\n");
        return false;
    }
    
    EVP_PKEY* pub_key = X509_get_pubkey(x509);
    EVP_MD_CTX *mdctx = NULL;
    mdctx = EVP_MD_CTX_create();
    EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, pub_key);
    EVP_DigestVerifyUpdate(mdctx, data.bytes(), data.size());
    int result = EVP_DigestVerifyFinal(mdctx, (unsigned char*)signature.bytes(), signature.size());
    
    X509_free(x509);
    EVP_PKEY_free(pub_key);
    EVP_MD_CTX_destroy(mdctx);
    return result > 0;
}

Data CryptoManager::sign_data(Data data, Data key) {
    BIO* bio = BIO_new_mem_buf(key.bytes(), key.size());
    
    EVP_PKEY* pkey;
    pkey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
    
    BIO_free(bio);
    
    if (!pkey) {
        LOG("Unable to parse private key in memory!");
        exit(-1);
    }
    
    EVP_MD_CTX *mdctx = NULL;
    mdctx = EVP_MD_CTX_create();
    EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, pkey);
    EVP_DigestSignUpdate(mdctx, data.bytes(), data.size());
    size_t slen;
    EVP_DigestSignFinal(mdctx, NULL, &slen);
    unsigned char* signature = (unsigned char*)malloc(slen);
    int result = EVP_DigestSignFinal(mdctx, signature, &slen);
    
    EVP_PKEY_free(pkey);
    EVP_MD_CTX_destroy(mdctx);
    
    if (result <= 0) {
        free(signature);
        LOG("Unable to sign data!");
        exit(-1);
    }
    
    Data signed_data = Data(signature, slen);
    free(signature);
    return signed_data;
}

Data CryptoManager::get_signature_from_cert(Data cert) {
    BIO* bio = BIO_new_mem_buf(cert.bytes(), cert.size());
    X509* x509 = PEM_read_bio_X509(bio, NULL, NULL, NULL);
    
    if (!x509) {
        LOG("Unable to parse certificate in memory!\n");
        exit(-1);
    }
    
#if (OPENSSL_VERSION_NUMBER < 0x10002000L)
    ASN1_BIT_STRING *asn_signature = x509->signature;
#elif (OPENSSL_VERSION_NUMBER < 0x10100000L)
    ASN1_BIT_STRING *asn_signature;
    X509_get0_signature(&asn_signature, NULL, x509);
#else
    const ASN1_BIT_STRING *asn_signature;
    X509_get0_signature(&asn_signature, NULL, x509);
#endif
    
    Data sig = Data(asn_signature->data, asn_signature->length);
    X509_free(x509);
    return sig;
}

Data CryptoManager::get_key_from_cert_key_pair(PCERT_KEY_PAIR cert_key_pair) {
    BIO* bio = BIO_new(BIO_s_mem());
    
#if (OPENSSL_VERSION_NUMBER < 0x10100000L)
    PEM_write_bio_PrivateKey(bio, cert_key_pair->pkey, NULL, NULL, 0, NULL, NULL);
#else
    PEM_write_bio_PrivateKey_traditional(bio, cert_key_pair->pkey, NULL, NULL, 0, NULL, NULL);
#endif
    
    BUF_MEM* mem;
    BIO_get_mem_ptr(bio, &mem);
    Data data = Data(mem->data, mem->length);
    BIO_free(bio);
    return data;
}

Data CryptoManager::get_p12_from_cert_key_pair(PCERT_KEY_PAIR cert_key_pair) {
    BIO* bio = BIO_new(BIO_s_mem());
    
    i2d_PKCS12_bio(bio, cert_key_pair->p12);
    
    BUF_MEM* mem;
    BIO_get_mem_ptr(bio, &mem);
    Data data = Data(mem->data, mem->length);
    BIO_free(bio);
    return data;
}

Data CryptoManager::get_cert_from_cert_key_pair(PCERT_KEY_PAIR cert_key_pair) {
    BIO* bio = BIO_new(BIO_s_mem());
    
    PEM_write_bio_X509(bio, cert_key_pair->x509);
    
    BUF_MEM* mem;
    BIO_get_mem_ptr(bio, &mem);
    Data data = Data(mem->data, mem->length);
    BIO_free(bio);
    return data;
}
