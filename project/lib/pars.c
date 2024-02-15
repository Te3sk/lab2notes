#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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
            else
            {
                // error handling
                printf("%srequestParser - invalid field code\n");
                exit(EXIT_FAILURE);
            }
            req->size++;
        }

        token = strtok(NULL, ";");
    }

    return req;
}

/*
### Description
    Check the format of the parameters send by user in the program calling and parse them in a string that have to be send to the server
### Parameters
    - `int argc` is the number of arguments the user wrote in the program calling
    - `char *argv[]` is an array of string with all the arguments
    NB: the first argument is always the name of the executable file
### Return value
    On success return a formatted string with all the infos
    On error/fail return a message error and NULL
*/
char *checkInputFormatNparser(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Too few parameters\n%s\n", USAGE_STRING);
        return NULL;
    }

    // TODO - set the right size
    char *request = (char *)malloc(sizeof(char) * 5000);

    int loan = 0;
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
            loan = 1;
        }
        else
        {
            printf("Wrong parameters format\n", USAGE_STRING);
            return NULL;
        }

        if (loan == 1)
        {
            strcat(request, "p");
        }
    }

    return request;
}