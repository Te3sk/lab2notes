#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "./socket/temp_sock"
#define THIS_PATH "client.c/"
#define BUFFER_SIZE 500 // TODO - understand how many bytes give
#define USAGE_STRING "Usage: ./client.c --filed=\"value\" -p\n\tonly one field per request, \n\tp indicates loan request"

#define MSG_QUERY 'Q'
#define MSG_LOAN 'L'
#define MSG_RECORD 'R'
#define MSG_NO 'N'
#define MSG_ERROR 'E'

#define SOCKET_PATH "./socket/temp_sock"

char *checkInputFormatNparser(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    char *parameters = checkInputFormatNparser(argc, argv);
    
    // @ temp test
    printf("parameters: |%s|\n", parameters);

    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(socket_fd == -1){
        // error handling
        perror(THIS_PATH "main - socket failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        // error handling
        perror(THIS_PATH "main - connect failed");
        exit(EXIT_FAILURE);
    }

    if(send(socket_fd, parameters, strlen(parameters), 0) == -1) {
        // error handling
        perror(THIS_PATH "main - send failed");
        exit(EXIT_FAILURE);
    }

    // @ temp test
    printf("messaggio inviato\n");

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