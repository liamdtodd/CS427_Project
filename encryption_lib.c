#include "encryption_lib.h"

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


