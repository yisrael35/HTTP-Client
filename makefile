all: client.c
	gcc -g -Wall client.c -o client
all-GDB: client.c
	gcc -g -Wall client.c -o client
