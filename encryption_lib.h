#ifndef _ENCRPYTION_LIB_H
#define _ENCRYPTION_LIB_H

struct Message{
    char msg[512];
    char sender_signature[256];
    char nonce[32];
};

RSA *RSA_from_file(char* file_path, int pub_priv);

#endif
