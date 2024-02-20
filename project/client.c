#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#define SOCKET_PATH "./socket/temp_sock"
#define THIS_PATH "client.c/"
#define BUFFER_SIZE 500 // TODO - understand how many bytes give
#define USAGE_STRING "Usage: ./client.c --filed=\"value\" -p\n\tonly one field per request, \n\tp indicates loan request"

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

char *checkInputFormatNparser(int argc, char *argv[], bool *loan);
void sendData(int socketFD, char type, char *data);
int send_int(int num, int fd);
int receive_int(int *num, int fd);

int main(int argc, char *argv[])
{
    bool loan;
    char *parameters = checkInputFormatNparser(argc, argv, &loan);
    
    // @ temp test
    printf("parameters: |%s|\n", parameters);

    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(socket_fd == -1){
        // error handling
        perror(THIS_PATH "main - socket failed");
        exit(EXIT_FAILURE);
    }

    // @ temp test
    printf("socket ok\n");

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // @ temp test
    printf("data structure ok\n");

    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        // error handling
        perror(THIS_PATH "main - connect failed");
        exit(EXIT_FAILURE);
    }

    // @ temp test
    printf("connect ok\n");
    
    sendData(socket_fd, loan?MSG_LOAN:MSG_QUERY, parameters);

    // if(send(socket_fd, parameters, strlen(parameters), 0) == -1) {
    //     // error handling
    //     perror(THIS_PATH "main - send failed");
    //     exit(EXIT_FAILURE);
    // }


    // @ temp test
    printf("messaggio inviato: |%s|\n", parameters);

    return 0;
}

/*
### Description
    Check if the format of the input values is right and parse them in a string for the request to the server
### Parameters
    - `int argc` and `char *argv[]` are the command line parameters
### Return value
    On success return the parsed string, on fail print the mistake and return `NULL`
*/
char *checkInputFormatNparser(int argc, char *argv[], bool *loan)
{
    if (argc < 2)
    {
        printf("Too few parameters\n%s\n", USAGE_STRING);
        return NULL;
    }

    // TODO - set the right size
    char *request = (char *)malloc(sizeof(char) * 5000);
    *loan = false;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] == '-')
        {
            char *eq = strchr(argv[i], '=');
            if (eq == NULL)
            {
                printf("Wrong parameters format\n", USAGE_STRING);
                return NULL;
            }

            // TODO - set the right sizes
            char *field_code = (char *)malloc(strlen(argv[i]) * sizeof(char));
            char *field_value = (char *)malloc(strlen(argv[i]) * sizeof(char));
            char *temp = (char *)malloc(strlen(argv[i]) * sizeof(char));

            field_code = argv[i] + 2;
            *eq = '\0';
            field_value = eq + 1;

            sprintf(temp, "%s:%s;", field_code, field_value);

            strcat(request, temp);
        }
        else if (argv[i][0] == '-')
        {
            *loan = true;
        }
        else
        {
            printf("Wrong parameters format\n", USAGE_STRING);
            return NULL;
        }
    }

    return request;
}

void sendData(int socketFD, char type, char *data){
    // TODO - send type
    if(send(socketFD, &type, sizeof(type), 0) == -1){
        // error handling
        perror(THIS_PATH "sendData - type sending failed");
        exit(EXIT_FAILURE);
    }
    // @ temp test
    printf("send type: %c\n", type);
    // TODO - send length
    char *length = (char*)malloc(10 * sizeof(char));
    if(length == NULL){
        // error handling
        perror("sendData - length allocation failed");
        exit(EXIT_FAILURE);
    }
    sprintf(length, "%d\0", strlen(data));
    if(send(socketFD, length, strlen(length), 0) == -1){
        // error handling
        perror(THIS_PATH "sendData - length sending failed");
        exit(EXIT_FAILURE);
    }
    // @ temp test
    printf("send length: %s\n", length);
    // TODO - send data
    if(send(socketFD, data, strlen(data), 0) == -1){
        // error handling
        perror(THIS_PATH"sendData - data sending failed");
        exit(EXIT_FAILURE);
    }
    // @ temp test
    printf("send data: %s\n", data);
}


int send_int(int num, int fd)
{
    // num from host byte order to network byte order
    int32_t conv = htonl(num);
    // to string for sending
    char *data = (char*)&conv;
    // size of bytes to send
    int left = sizeof(conv);
    int rc;
    do {
        rc = write(fd, data, left);
        if(rc == -1){
            // error handling
            perror(THIS_PATH"send_int - write failed");
            exit(EXIT_FAILURE);
        }
        else {
            // update data pointer and remaining bytes
            data += rc;
            left -= rc;
        }
    }
    while (left > 0); // continue untill it send all bytes
    return 0;
}
int receive_int(int *num, int fd)
{
    // network byte order type
    int32_t ret;
    // char for reading from socket
    char *data = (char*)&ret;
    // size of bytes to receive
    int left = sizeof(ret);
    int rc;
    do {
        rc = read(fd, data, left);
        if(rc == -1){
            // error handling
            perror(THIS_PATH"receive_int - read failed");
            exit(EXIT_FAILURE);
        }
        else {
            // update data pointer and remaining bytes
            data += rc;
            left -= rc;
        }
    }
    while (left > 0); // continue untill it send all bytes
    // from network bytes order to host bytes order
    *num = ntohl(ret);
    return 0;
}