client: client.c encryption_lib.c
	gcc -lcrypto client.c encryption_lib.c -o client