/************************
 * Code from first main function is borrowed from:
 * https://beej.us/guide/bgnet/html/#client-server-background
 *
 * Description: Runs the client-side of a messaging system
 * ***********************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT "51000" //port of client
#define MAXDATASIZE 100 //max number of data

void* get_in_addr(struct sockaddr *sa) {
        if (sa->sa_family == AF_INET) {
                return &(((struct sockaddr_in*)sa)->sin_addr);
        }
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void client_loop(int sockfd, char *user_name, char* pkey) {
    char *message = NULL;
    size_t len = 0;
    ssize_t message_size;
    int numbytes, n;
    char buf[MAXDATASIZE];
    char user_message[256];
    char* ctxt = NULL;

    while (1) {
        memset(buf, '\0', MAXDATASIZE);
        memset(user_message, '\0', 256);
        

        while((message_size = getline(&message, &len, stdin)) != -1) {
            // concatenate the message with the user_name
            strcpy(user_message, user_name);
            strcat(user_message, ": ");
            strcat(user_message, message);

	        // encrypt the user's message
	        int ctxt_length = public_encrypt((unsigned char *) user_message, strlen(user_message) + 1, pkey, (unsigned char *) ctxt, sockfd);

            // ctxt gets malloc'd in public_encrypt
            free(ctxt);
            ctxt = NULL;

            if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
                perror("recv");
                exit(1);
            }

        }

        free(message);
        message = NULL;
    }
}

int main(int argc, char* argv[]) {
        int sockfd, numbytes;
        char buf[MAXDATASIZE];
        struct addrinfo hints, *servinfo, *p;
        int rv;
        char s[INET6_ADDRSTRLEN];

        // Seed the time to use with RSA
        srand(time(NULL));

        if (argc != 5) {
                fprintf(stderr, "usage: client hostname port username pubkey\n");
                exit(1);
        }

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 1;
        }
        for (p = servinfo; p != NULL; p = p->ai_next) {
                if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                        perror("client: socket");
                        continue;
                }

                if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                        close(sockfd);
                        perror("client: connect");
                        continue;
                }

                break;
        }

        if (p == NULL) {
                fprintf(stderr, "client: failed to connect\n");
                return 2;
        }

        inet_ntop(p->ai_family, get_in_addr((struct sockaddr*) p->ai_addr), s, sizeof s);
        printf("client: connecting to %s\n", s);

        freeaddrinfo(servinfo);

        // enter the client loop to encrypt on a server
        client_loop(sockfd, argv[3], argv[4]);

        close(sockfd);

        return 0;
}
