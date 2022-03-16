#ifndef _ENCRPYTION_LIB_H
#define _ENCRYPTION_LIB_H

#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>

struct Message{
    char msg[512];
    char sender_signature[256];
    char nonce[32];
};

RSA *RSA_from_file(char* file_path, int pub_priv);
//int RSA_public_encrypt(int length, unsigned char* ptxt, unsigned char* ctxt, RSA *rsa, int pad);
//int RSA_private_decrypt(int length, unsigned char* ctxt, unsigned char* ptxt, RSA *rsa, int pad);
int public_encrypt(unsigned char* data, int data_len, unsigned char* key, unsigned char* encrypted, int sockfd);
int private_decrypt(unsigned char* enc_data, int data_len, unsigned char* key, unsigned char* decrypted);

#endif
