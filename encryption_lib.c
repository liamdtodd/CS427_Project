#include "encryption_lib.h"
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <stdio.h>

RSA *RSA_from_file(char* file_path, int pub_priv) {
    FILE* fp = fopen(file_path, "rb");

    RSA *rsa_context = RSA_new();

    if (pub_priv == 1) {
        rsa_context = PEM_read_RSA_PUBKEY(fp, &rsa_context, NULL, NULL);
    }
    else {
        rsa_context = PEM_read_RSAPrivateKey(fp, $rsa_context, NULL, NULL);
    }

    return rsa_context;
}

int RSA_public_encrypt(int flen, unsigned char* from, unsigned char* to, RSA *rsa, int padding) {

}

int RSA_private_decrypt(int flen, unsigned char* from, unsigned char* to, RSA *rsa, int padding) {

}

int padding = RSA_PKCS1_PADDING;

int public_encrypt(unsigned char* data, int data_len, unsigned char* file_path, unsigned char* encrypted) {
	RSA *rsa = RSA_from_file(file_path, 1);		//file name?
	int result = RSA_public_encrypt(data_len, data, encrypted, rsa, padding);
	return result;
}

int private_decrypt(unsigned char* data, int data_len, unsigned char* file_path, unsigned char* decrypted) {
	RSA *rsa = RSA_from_file(file_path, 0);		//file name?
	int result = RSA_private_decrypt(data_len, enc_data, decrypted, rsa, padding);
	return result;
}	
