#include <stdio.h>
#include <stdlib.h>

#define SOCKET_PATH "./socket/temp_sock"
#define THIS_PATH "client.c/"
#define BUFFER_SIZE 500 // TODO - understand how many bytes give

int checkInputFormat(int argc, char *argv[]);
char *requestParser(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    checkInputFormat(argc, argv);

    return 0;
}

int checkInputFormat(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Too few parameters\nUsage: ./client.c --filed=\"value\" -p\n\tonly one field per request, \n\tp indicates loan request");
    }

    return 0;
}