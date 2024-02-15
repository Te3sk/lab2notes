#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "lib/queue.h"
#include "lib/bib_ds.h"

#define SOCKET_PATH "./socket/temp_sock"
#define THIS_PATH "server.c/"
#define MAX_CLIENTS 10 // temp, it must be 40
#define MAX_LENGTH 500 // TODO - understand how many bytes give
// TODO - temp, W have to be insert by user in program launch
#define W 10

typedef struct WorkerArgs
{
    Queue *q;
    BibData *bib;
} WorkerArgs;

typedef struct Request
{
    char **field_codes;
    char **field_values;
    int size;
    bool loan;
} Request;

void *worker(void *arg);
Request *requestParser(char *string);

int main()
{
    Request *req = (Request *)malloc(sizeof(Request));

    char temp[] = "author:ciccio;title:pippo;p";

    // @ temp test
    printf("%s\n", temp);

    requestParser(temp);

    // BibData *bib = createBibData("bibData/bibFake.txt");

    // if(bib == NULL){
    //     // error handling
    //     exit(EXIT_FAILURE);
    // }

    // // * socket creation
    // int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    // if (server_socket == -1)
    // {
    //     // error handling
    //     perror("socket creation failed");
    //     exit(EXIT_FAILURE);
    // }

    // // * socket address definition
    // struct sockaddr_un server_address;
    // server_address.sun_family = AF_UNIX;
    // strcpy(server_address.sun_path, SOCKET_PATH);

    // // * association of the socket to the address
    // if(bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1){
    //     // error handling
    //     perror("bind failed");
    //     exit(EXIT_FAILURE);
    // }

    // // * Listen to the socket for incoming connections
    // if(listen(server_socket, MAX_CLIENTS) == -1){
    //     // error handling
    //     perror("listen failed");
    //     exit(EXIT_FAILURE);
    // }

    // // @ temp test
    // printf("Il server è pronto e in ascolto (fd: %d)\n", server_socket);

    // // TODO - queue init
    // Queue *q;
    // init(q);
    // // @ temp test
    // printf("queue initalized\n");

    // // TODO - thread creation

    // // * main cycle
    // while(1) {
    //     int client_fd = accept(server_socket, NULL, NULL);
    //     if(client_fd == -1){
    //         // error handling
    //         perror("accept failed");
    //         exit(EXIT_FAILURE);
    //     }

    //     // @ temp test
    //     printf("Nuova connessione accettata(client fd: %d)\n", client_fd);

    //     // TODO - put request in the queue
    // }

    // // TODO - poi dovrà essere gestito con il segnale di terminazione
    // close(server_socket);
    // unlink(SOCKET_PATH);

    return 0;
}

void *worker(void *arg)
{
    Queue *queue = ((WorkerArgs *)arg)->q;
    BibData *bib = ((WorkerArgs *)arg)->bib;
    // @ temp test
    printf("\tenter in worker func\n");

    // TODO - error for this
    // int clientFD = pop(queue);
    // @ temp test
    // printf("\tclient fd = %d\n", clientFD);

    char *buffer = (char *)malloc(MAX_LENGTH * sizeof(char));
    // int bytesread = read(clientFD, buffer, MAX_LENGTH);
    // if(bytesread == -1){
    //     // error handling
    //     perror(THIS_PATH "worker - read failed");
    //     exit(EXIT_FAILURE);
    // }

    // @ temp test
    // printf("\tclient send: %s\n", buffer);

    // TODO - waiting for lib/bib_ds.c/requestFormatCheck dev
    // if(requestFormatCheck(buffer) == comparig value){
    //     // error handling
    //     perror("worker - wrong request format");
    //     exit(EXIT_FAILURE);
    // }
}

void free_request(Request *request) {
    if (request == NULL) return;
    for (int i = 0; i < request->size; ++i) {
        free(request->field_codes[i]);
        free(request->field_values[i]);
    }
    free(request->field_codes);
    free(request->field_values);
    free(request);
}


Request *requestParser(char *string) {
    // @ temp test
    printf("enter in requestParser function\n");

    if(string == NULL){
        // error handling
        perror(THIS_PATH "requestParser - error with the string to parse");
        return NULL;
    }

    Request *req = (Request *)malloc(sizeof(Request));
    if(req == NULL){
        // error handling
        perror(THIS_PATH "requestParser - req allocation failed");
        exit(EXIT_FAILURE);
    }

    // @ temp test
    printf("req ok\n");

    req->field_codes = NULL;
    req->field_values = NULL;
    req->size = 0;
    req->loan = false;

    // @ temp test
    printf("req initialized\n");

    char *token = strtok(string, ";");
    while (token != NULL) {
        // @ temp test
        printf("---- current token : %s -----\n", token);
        if (strcmp(token, "p") == 0) {
            req->loan = true;
            // @ temp test
            printf("\tloan = true\n");
        } else {
            // @ temp test
            printf("\tno p parameter\n");
            req->field_codes = (char **)realloc(req->field_codes, (req->size + 1) * sizeof(char *));
            req->field_values = (char **)realloc(req->field_values, (req->size + 1) * sizeof(char *));
            if(req->field_codes == NULL || req->field_values == NULL){
                // error handling
                perror(THIS_PATH "requestParser - allocation of req->field_codes||values failed");
                free_request(req);
                exit(EXIT_FAILURE);
            }
            // @ temp test
            printf("\tfields[][] ok\n");

            char *pv = strchr(token, ':');
            if(pv == NULL){
                // error handling
                perror(THIS_PATH "requestParser - invalid field");
                exit(EXIT_FAILURE);
            }

            // @ temp test
            printf("pv ok\n");

            req->field_codes[req->size] = (char *)malloc((pv - token + 1) * sizeof(char));
            req->field_values[req->size] = strdup(pv + 1);
            if(req->field_codes[req->size] == NULL || req->field_values[req->size] == NULL){
                // error handling
                perror(THIS_PATH "requestParser - allocation of req->field_codes[i]||values[i] failed");
                free_request(req);
                exit(EXIT_FAILURE);
            }

            // @ temp test
            printf("\tcodes[size] ok\n");
            // @ temp test
            printf("\tvalues[size] = %s\n", req->field_values[req->size]);

            strncpy(req->field_codes[req->size], token, pv - token);
            // @ temp test
            req->field_codes[req->size][pv - token] = '\0';
            printf("\tcodes[size] = %s\n", req->field_codes[req->size]);
            req->size++;
            // @ temp test
            printf("\tsize = %d\n", req->size);
        }

        token = strtok(NULL, ";");
    }

    for (int i = 0; i < req->size; i++) {
        printf("%s\t:\t%s\n", req->field_codes[i], req->field_values[i]);
    }
    printf("loan\t:\t%s\n", req->loan ? "true" : "false");

    return req;
}





// Request *requestParser(char *string)
// {
//     // @ temp test
//     printf("----enter in function----\n");

//     Request *req = (Request *)malloc(sizeof(Request));
//     req->field = (char **)malloc(sizeof(char *));
//     req->size = 0;
//     req->loan = false;

//     // @ temp test
//     printf("request variable initialized:\n\tfield[0]:%s, size:%d, loan:%d\n", req->field[0], req->size, req->loan);

//     char *sep = strchr(string, ';');
//     *sep = '\0';
//     sep = sep + 1;


//     // @ temp test
//     printf("first sep = %s\t|\tfirst string (after sep) = %s\n", sep, string);
    
    
//     if (string[0] == 'p')
//     {
//         // @ temp test
//         printf("current parameter IS p\n");
//         req->loan = true;
//     }
//     else
//     {
//         // @ temp test
//         printf("current parameter is NOT p\n");
//         req->field[req->size] = string;
//         req->size++;
//         // @ temp test
//         printf("%s\n", req->field[0]);
//     }
//     string = sep;
//     while (sep != NULL)
//     {
//         if (string[0] == 'p')
//         {
//             req->loan = true;
//         }
//         else
//         {
//             req->field[req->size] = string;
//             // @ temp test
//             printf("%s\n", req->field[req->size]);
//         }
//         req->size++;
//         string = sep;
//         sep = strchr(string, ';');
//     }

//     return NULL;
// }