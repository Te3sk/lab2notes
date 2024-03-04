#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "pars.h"

#define THIS_PATH "lib/pars.c/"
#define USAGE_STRING "Usage: ./client.c --filed=\"value\" -p\n\tonly one field per request, \n\tp indicates loan request"

/*
### Description
    free the memory of an entire `Request` data structure
### Parameters
    - `Request *request` is the data structure to eliminate
*/
void free_request(Request *request)
{
    if (request == NULL)
        return;
    for (int i = 0; i < request->size; ++i)
    {
        free(request->field_codes[i]);
        free(request->field_values[i]);
    }
    free(request->field_codes);
    free(request->field_values);
    free(request);
}

/*
### Description
    Parse the string send by client and put the results in a `Request` type data structure
### Parameters
    - `char *string` is the string to parse
### Return value
    On success return the `Request` data structure filled by the infos
    On fail / error print an error message and return NULL
*/
Request *requestParser(char *string)
{
    if (string == NULL)
    {
        // error handling
        perror(THIS_PATH "requestParser - error with the string to parse");
        return NULL;
    }

    Request *req = (Request *)malloc(sizeof(Request));
    if (req == NULL)
    {
        // error handling
        perror(THIS_PATH "requestParser - req allocation failed");
        exit(EXIT_FAILURE);
    }

    req->field_codes = NULL;
    req->field_values = NULL;
    req->size = 0;
    req->loan = false;

    char *token = strtok(string, ";");
    while (token != NULL)
    {
        if (strcmp(token, "p") == 0)
        {
            req->loan = true;
        }
        else
        {
            req->field_codes = (char **)realloc(req->field_codes, (req->size + 1) * sizeof(char *));
            req->field_values = (char **)realloc(req->field_values, (req->size + 1) * sizeof(char *));
            if (req->field_codes == NULL || req->field_values == NULL)
            {
                // error handling
                perror(THIS_PATH "requestParser - allocation of req->field_codes||values failed");
                free_request(req);
                exit(EXIT_FAILURE);
            }

            char *pv = strchr(token, ':');
            if (pv == NULL)
            {
                // error handling
                perror(THIS_PATH "requestParser - invalid field");
                exit(EXIT_FAILURE);
            }

            char *temp_field_code = (char *)malloc((pv - token) * sizeof(char));

            req->field_codes[req->size] = (char *)malloc((pv - token + 1) * sizeof(char));
            req->field_values[req->size] = strdup(pv + 1);
            if (temp_field_code == NULL || req->field_values[req->size] == NULL)
            {
                // error handling
                perror(THIS_PATH "requestParser - allocation of req->field_codes[i]||values[i] failed");
                free_request(req);
                exit(EXIT_FAILURE);
            }

            strncpy(temp_field_code, token, pv - token);

            if (strcmp(temp_field_code, "author") == 0)
            {
                req->field_codes[req->size] = "autore";
            }
            else if (strcmp(temp_field_code, "title") == 0)
            {
                req->field_codes[req->size] = "titolo";
            }
            else if (strcmp(temp_field_code, "year") == 0)
            {
                req->field_codes[req->size] = "anno";
            }
            else if (strcmp(temp_field_code, "editor") == 0)
            {
                req->field_codes[req->size] = "editore";
            }
            else if (strcmp(temp_field_code, "collocation") == 0)
            {
                req->field_codes[req->size] = "collocazione";
            }
            else if (strcmp(temp_field_code, "physical_description") == 0)
            {
                req->field_codes[req->size] = "descrizione_fisica";
            }
            // ramo else default tolto, se non è uno di quelli (in en), lo lascia com'è
            // // else
            // // {
            // //     // error handling
            // //     printf("%srequestParser - invalid field code\n", THIS_PATH);
            // //     exit(EXIT_FAILURE);
            // // }
            req->size++;
        }

        token = strtok(NULL, ";");
    }

    return req;
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
                printf("Wrong parameters format, Usage:\n\t%s\n", USAGE_STRING);
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
            printf("Wrong parameters format, Usage:\n\t%s\n", USAGE_STRING);
            return NULL;
        }
    }

    request[strlen(request) - 1] = '\0';

    return request;
}

/*
### Description
    read an integer from a socket using network byte order type
### Parameters
    - `int fd` is the file descriptor from which read the integer
### Return value
    on success return the int readed, on fail print an error msg and exit
*/
int receive_int(int fd)
{
    // network byte order type
    int32_t ret;
    // char for reading from socket
    char *data = (char *)&ret;
    // size of bytes to receive
    int left = sizeof(ret);
    int rc;
    do
    {
        rc = read(fd, data, left);
        if (rc == -1)
        {
            // error handling
            perror(THIS_PATH "receive_int - read failed");
            exit(EXIT_FAILURE);
        }
        else
        {
            // update data pointer and remaining bytes
            data += rc;
            left -= rc;
        }
    } while (left > 0); // continue untill it send all bytes
    // from network bytes order to host bytes order
    return ntohl(ret);
}

/*
### Description
    send an integer data on a socket using network byte order type
### Parameters
    - `int num` is the integer to send
    - `int fd` is the file descriptor of the socket on which send the integere
### Note
    on fail send an error msg and exit
*/
void send_int(int num, int fd)
{
    // num from host byte order to network byte order
    int32_t conv = htonl(num);
    // to string for sending
    char *data = (char *)&conv;
    // size of bytes to send
    int left = sizeof(conv);
    int rc;
    do
    {
        rc = write(fd, data, left);
        if (rc == -1)
        {
            // error handling
            perror(THIS_PATH "send_int - write failed");
            exit(EXIT_FAILURE);
        }
        else
        {
            // update data pointer and remaining bytes
            data += rc;
            left -= rc;
        }
    } while (left > 0); // continue untill it send all bytes
}

// TODO - desc
time_t date_extract(char *date) {
    struct tm tm_data;
    memset(&tm_data, 0, sizeof(struct tm));
    strptime(date, "%d-%m-%Y %H:%M:%S", tm_data);
    time_t converted = mktime(&tm_data);
    if(converted == (time_t)-1){
        // error handling
        perror(THIS_PATH"date_extract - mktime failed");
        exit(EXIT_FAILURE);
    }
    return converted;
}