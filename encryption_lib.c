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
        rsa_context = PEM_read_RSAPrivateKey(fp, &rsa_context, NULL, NULL);
    }

    return rsa_context;
}

//int RSA_public_encrypt(int length, unsigned char* ptxt, unsigned char* ctxt, RSA *rsa, int padding) {

//	return 0;
//}

//int RSA_private_decrypt(int length, unsigned char* ctxt, unsigned char* ptxt, RSA *rsa, int padding) {

//	return 0;
//}


int public_encrypt(unsigned char* data, int data_len, unsigned char* file_path,
    unsigned char* encrypted, int sockfd) {
	RSA *rsa = RSA_from_file(file_path, 1);		//file name?
    
    // Allocate the proper size for our ctext
    encrypted = (unsigned char *) malloc(RSA_size(rsa));
	int result = RSA_public_encrypt(data_len, data, encrypted, rsa, RSA_PKCS1_OAEP_PADDING);
    
    if (result == -1) {
        printf("%s\n", "Error RSA encrypt pub");
        char err[130];
        ERR_load_crypto_strings();
        ERR_error_string(ERR_get_error(), err);
        fprintf(stderr, "Error encrypting message: %s\n", err);
    }

	// moved send into here to actually send a ctext
    send(sockfd, (const void *) encrypted, result, 0);

    
    return result;
}

int private_decrypt(unsigned char* data, int data_len, unsigned char* file_path, unsigned char* decrypted) {
	RSA *rsa = RSA_from_file(file_path, 0);		//file name?
	int result = RSA_private_decrypt(data_len, data, decrypted, rsa, RSA_PKCS1_PADDING);
	return result;
}

// Old method of delivering a message to stdout
#if 0
void *deliver_ctext(void *arg) {
    FILE* ctext_fd;
    int i = 0, j = 0;
    struct stat file_info;
    long last_modifed_time = -1;
    char *buff = NULL;
    size_t line_buf_size = 0;
    ssize_t line_size;

    // Find the most recent ctext in the log file
    while (1) {
        ctext_fd = fopen("log.txt", "r");
        line_size = 1;
        j = 1;
        while (line_size >= 0) {
            line_size = getline(&buff, &line_buf_size, ctext_fd);
            j++;
            if (j > i) {
                printf("%s   ", buff);
            }
        }
        // Update the i (current line) value and close the file
        i = j;
        fclose(ctext_fd);
    }

}
#endif
