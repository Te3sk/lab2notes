#ifndef PARS_H
#define PARS_H

#include <stdbool.h>
/*
### Description
    Data structure type for a request by client. The server use it for more semplicity when searching in the record file
### Field
    - `char **field_codes` is an array of string with the identifiers of the fileds (autore, titolo, ...)
    - `char **field_values` is an array of string with the values ​​corresponding to field_codes by index
    - `int size` is the number of field_codes:field_values in the Request
    - `bool loan` is true if the client ask for a loan, false otherwise
*/
typedef struct Request
{
    char **field_codes;
    char **field_values;
    int size;
    bool loan;
    int senderFD;
} Request;

/*
### Description
    Data structure type for a request by client. The server use it for more semplicity when searching in the record file
### Field
    - `int pos` is an array of integer with the position in the BibData of the records that match response
    - `int size` is the number of field_codes:field_values in the Request
    - `bool loan` is true if the loan is aviable, false otherwise
*/
typedef struct Response {
    int *pos;
    int size;
    bool loan;
} Response;

void free_request(Request *request);
Request *requestParser(char *string);
char *checkInputFormatNparser(int argc, char *argv[], bool *loan);
int receive_int(int fd);
void send_int(int num, int fd);
time_t date_extract(char *date);

#endif