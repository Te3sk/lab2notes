#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <arpa/inet.h>

#include "lib/pars.h"

#define SOCKET_PATH "./socket/temp_sock"
#define THIS_PATH "client.c/"
#define BUFFER_SIZE 500 // TODO - understand how many bytes give
#define USAGE_STRING "Usage: ./client.c --filed=\"value\" -p\n\tonly one field per request, \n\tp indicates loan request"
#define CONFIG_FILE "./config/bib.conf"

// Messaggio per richiedere i record che contengono alcune parole specifiche in alcuni campi
#define MSG_QUERY 'Q'
// Messaggio per richiedere il prestito di tutti i record che contengono alcune parole specifiche in alcuni campi
#define MSG_LOAN 'L'
// Messaggio di invio record. Il campo buffer contiene il record completo come stringa correttamente terminata da '\0'.
#define MSG_RECORD 'R'
// Messaggio di risposta negativa. Con questo messaggio il server segnala che non ci sono record che verificano la query
#define MSG_NO 'N'
// MSG ERROR Messaggio di errore. Questo tipo di messaggio viene spedito quando si è verificato un errore nel processare la richiesta del client.
#define MSG_ERROR 'E'

#define SOCKET_PATH "./socket/temp_sock"

typedef struct
{
    char *name;
    char *socket_path;
} ServerInfo;

void sendData(int socketFD, char type, char *data);
char *readData(int socketFD);
ServerInfo *readServerInfo(int *count);

int main(int argc, char *argv[])
{

    bool loan;
    char *parameters = checkInputFormatNparser(argc, argv, &loan);

    // read servers infos from conf file
    int serverNum;
    ServerInfo *serverInfo = readServerInfo(&serverNum);

    // @ temp test
    printf("letti %d server\n", serverNum);

    for (int i = 0; i < serverNum; i++)
    {
        int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (socket_fd == -1)
        {
            // error handling
            perror(THIS_PATH "main - socket failed");
            exit(EXIT_FAILURE);
        }

        // @ temp test
        printf("try to connect with %s on %s\n", serverInfo[i].name, serverInfo[i].socket_path );
        struct sockaddr_un server_addr;
        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, serverInfo[i].socket_path);

        if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        {
            // error handling
            perror(THIS_PATH "main - connect failed");
            exit(EXIT_FAILURE);
        }

        sendData(socket_fd, loan ? MSG_LOAN : MSG_QUERY, parameters);

        char *response = readData(socket_fd);

        if (response != NULL)
        {
            // TODO - capire formato messaggio finale e cambiare
            printf("Dal server %s:\n\t%s\n", serverInfo[i].name, response);
        }

        close(socket_fd);
    }

    return 0;
}

/*
### Description
    Send the data to the server with the right comunication protocol: type -> length -> data
### Parameters
    - `int socketFD` is the file descriptor
    - `char type` is the char with the type of msg for the client: MSG_RECORD, MSG_NO, MSG_ERROR
    - `char *data` is the msg for the client.
        on search success contain the records finded
        on fail or error is empty
*/
void sendData(int socketFD, char type, char *data)
{
    // send type
    if (send(socketFD, &type, sizeof(type), 0) == -1)
    {
        // error handling
        perror(THIS_PATH "sendData - type sending failed");
        exit(EXIT_FAILURE);
    }
    // send length
    send_int(strlen(data), socketFD);
    // send data
    if (send(socketFD, data, strlen(data), 0) == -1)
    {
        // error handling
        perror(THIS_PATH "sendData - data sending failed");
        exit(EXIT_FAILURE);
    }
}

/*
### Description
    Read data (request) from server whith the right comunication protocol: type -> length -> data
### Parameters
    - `int socketFD` is the file descriptor
### Return value
    On search success (type MSG_RECORD) return a pointer to the string with the response
    On search fail (type MSG_NO) print the msg "" (?) and return NULL
    On error with the request (type MSG_ERROR) print the msg "" (?) and return NULL
*/
char *readData(int socketFD)
{
    char type;
    int bytesread = read(socketFD, &type, sizeof(type));
    if (bytesread == -1)
    {
        // error handling
        perror(THIS_PATH "readData - type reading failed");
        exit(EXIT_FAILURE);
    }

    if (type == MSG_NO)
    {
        // TODO - capire se bisogna stampare qualcosa o niente e in caso fallo (controlla coerenza con la desc)
        printf("Nessun record trovato\n");
        return NULL;
    }
    else if (type == MSG_RECORD)
    {
        int length = receive_int(socketFD);
        char *buffer = (char *)malloc(length * sizeof(char));
        if (read(socketFD, buffer, length) == -1)
        {
            // error handling
            perror(THIS_PATH "readData - buffer read failed");
            exit(EXIT_FAILURE);
        }
        return buffer;
    }
    else if (type == MSG_ERROR)
    {
        // @ temp test
        printf("MSG_ERROR: hai mandato una richiesta non valida\n");
        // TODO - capire se bisogna stampare qualcosa o niente e in caso fallo (controlla coerenza con la desc)
        return NULL;
    }
    else
    {
        printf("ERROR: invalid type\n");
        exit(EXIT_FAILURE);
    }
}

// TODO - desc
ServerInfo *readServerInfo(int *count)
{
    FILE *config_file = fopen(CONFIG_FILE, "r");
    if (config_file == NULL)
    {
        // error handling
        perror(THIS_PATH "readServerInfo - fopen failed");
        exit(EXIT_FAILURE);
    }

    *count = 0;
    ServerInfo *serverInfo = (ServerInfo *)malloc(sizeof(ServerInfo) * (*count + 1));
    if (serverInfo == NULL)
    {
        // error handling
        perror(THIS_PATH "readServerInfo - serverInfo allocation failed");
        exit(EXIT_FAILURE);
    }
    char*temp_name = (char*)malloc(sizeof(char) * 100);
    char*temp_path = (char*)malloc(sizeof(char) * 100);

    // TODO - capire perché si invertono nome e path
    while (fscanf(config_file, "%s %s", temp_path, temp_name) == 2)
    {
        // @ temp test
        printf("\tname:%s|path;%s\n", temp_name, temp_path);
        serverInfo[*count].name = (char *)malloc(sizeof(char) * (strlen(temp_name) + 1));
        strcpy(serverInfo[*count].name, temp_name);
        serverInfo[*count].name[strlen(temp_name)] = '\0';
        serverInfo[*count].socket_path = (char *)malloc(sizeof(char) * (strlen(temp_path) + 1));
        strcpy(serverInfo[*count].socket_path, temp_path);
        serverInfo[*count].socket_path[strlen(temp_path)] = '\0';
        // @ temp test
        printf("letto %s %s\n", serverInfo[*count].name, serverInfo[*count].socket_path);
        (*count)++;
        serverInfo = (ServerInfo *)realloc(serverInfo, sizeof(ServerInfo) * (*count + 1));
    }

    fclose(config_file);
    // @ temp test
    return serverInfo;
}