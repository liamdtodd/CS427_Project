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
#include <time.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include "encryption_lib.h"

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
    int bytes_recvd, n;
    char buf[MAXDATASIZE];
    char user_message[256];
    char* ctxt = NULL;

    while (1) {
        memset(buf, '\0', MAXDATASIZE);
        memset(user_message, '\0', 256);

        printf("%s: ", user_name);
        message_size = getline(&message, &len, stdin);

        strcpy(user_message, user_name);
        strcat(user_message, ": ");
        strcat(user_message, message);

        if (strcmp(message, "exit()\n") == 0) {
                printf("exiting program\n");
                break;
        }

        //using time as associated data, concatenating time to the ctxt
        time_t timer;
        struct tm y2k = {0};
        double seconds;
        char timestr[32];

        time(&timer);

        y2k.tm_hour = 0;
        y2k.tm_min = 0;
        y2k.tm_sec = 0;
        y2k.tm_year = 100;
        y2k.tm_mon = 0;
        y2k.tm_mday = 1;				//setting time at Jan 1, 2000

        seconds = difftime(timer, mktime(&y2k));	//calculating seconds elapsed since Jan 1, 2000

        int sec = seconds / 1;				//turning the double into an integer

        sprintf(timestr, "%d", sec); 			//converts the int 'sec' into a string
	strcat(user_message, timestr);			//concatenating the time to the ptxt
        // send/encrypt with public_encrypt

        int ctxt_length = public_encrypt((unsigned char *) user_message, strlen(user_message) + 1, pkey, (unsigned char *) ctxt, sockfd);

        bytes_recvd = recv(sockfd, buf, MAXDATASIZE - 1, 0);

        if (bytes_recvd == -1) {
                perror("recv:");
                break;
        }
        // Sleep for 1 second so our timestamping method works
        sleep(1);

        // ctxt gets malloc'd in public_encrypt
        free(ctxt);
        ctxt = NULL;
    }
        free(message);
        message = NULL;
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
