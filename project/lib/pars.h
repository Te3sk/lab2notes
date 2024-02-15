/*
### Description
    Data structure type for a request by client. The server use it for more semplicity when searching in the record file
*/
typedef struct Request
{
    char **field_codes;
    char **field_values;
    int size;
    bool loan;
} Request;

typedef struct Response {
    char **records;
    int size;
} Response;

void free_request(Request *request);
Request *requestParser(char *string);
char *checkInputFormatNparser(int argc, char *argv[]);
