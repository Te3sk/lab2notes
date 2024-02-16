#define _GNU_SOURCE

#include "bib_ds.h"

#define MAX_LENGTH 500      // TODO - understand how many bytes give
#define MAX_FIELD_LENGTH 30 // TODO - understand how many bytes give
#define THIS_PATH "lib/bib_ds.c/"

/*
### Description
    checks, line by line, if the file is empty and its format
### Parameters
    'path' is the path of the txt file
### Return value
    If the file is totally right (not empty and right format) return 1
    If the file is empty or of the wrong format return NULL
*/
FILE *fileFormatCheck(char *path)
{

    // open the file
    FILE *fp = fopen(path, "rb");
    // @ temp test
    if (fp == NULL)
    {
        // error handling
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }

    // read the file line by line
    char *buffer = (char *)malloc(sizeof(char) * MAX_LENGTH);
    if (buffer == NULL)
    {
        // error handling
        perror(THIS_PATH "/fileFormatCheck - buffer allocation failed");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, MAX_LENGTH, fp) != NULL)
    {
        if (strlen(buffer) == 0)
        {
            continue;
        }
        buffer[strlen(buffer) - 1] = '\0';
        char *token = strtok(buffer, ";");

        while (token != NULL)
        {
            int i = 0;
            // delete spaces char
            while (token[0] == ' ')
            {
                memmove(token, token + 1, strlen(token));
            }

            if (strlen(token) != 0)
            {
                token[strlen(token) - 1] = '\0';

                if ((strstr(token, ":") == NULL) || ((strchr(token, ',')) && ((strchr(token, ',') - token) < (strchr(token, ':') - token))))
                {
                    printf("wrong file format\n");
                    return NULL;
                }
            }
            token = strtok(NULL, ";");
        }
    }

    return fp;
}

/*
### Description
    Creation of data structure give the path of a txt file
### Darameters
    'path' is the path of the txt file
### Return value
    On success return a pointer to a BibData variable (char** data + int size)
    filled with the data
    if the file is of the wrong format or empty return NULL
*/
BibData *createBibData(char *path)
{

    // create a new BibData object
    BibData *bib = malloc(sizeof(BibData));

    // open the file
    FILE *fp = fileFormatCheck(path);

    if (fp == NULL)
    {
        // error handling
        return NULL;
    }

    // go back to the beginning of the file
    rewind(fp);

    // allocate memory for the book array
    bib->book = malloc(bib->size * sizeof(char *));
    if (bib->book == NULL)
    {
        // error handling
        perror(THIS_PATH "/createBibData - bib->book allocation failed");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    bib->size = 0;

    bib->book[bib->size] = (char *)malloc((MAX_LENGTH + 1) * sizeof(char));
    while (fgets(bib->book[bib->size], MAX_LENGTH, fp) != NULL)
    {
        bib->size++;
        bib->book[bib->size] = (char *)malloc((MAX_LENGTH + 1) * sizeof(char));
    }

    return bib;
}

/*
### Description
    Recorsive function check if a single record match with a request
### Parameters
    - `int pos` is the position of req based on recursive iteration (at first call by another func is 0)
    - `char *record` is the single record extract from a bibData
    - `Request *req` is the current request
### Return value
    return `true` if the request match the record, `false` otherwise
*/
bool recordMatch(char *record, Request *req)
{
    bool found = true;
    char *recordCopy = (char *)malloc(strlen(record) * sizeof(char));
    for (int i = 0; i < req->size; i++)
    {
        bool subfound = false;
        strcpy(recordCopy, record);
        // @ temp test
        printf("%s\n", recordCopy);
        printf("->RECORD MATCH - pos : %d, field : %s, value : %s\n", i, req->field_codes[i], req->field_values[i]);
        // per ogni coppia campo:valore nel record
        char *token = strtok(recordCopy, ";");
        // @ temp test
        printf("%d\n%s\n", (token != NULL), token);
        while (token != NULL)
        {
            // confronta il campo con quello nella richiesta
            char *field = strtok(token, ":");
            char *value = strtok(NULL, ":");

            if (field != NULL && value != NULL)
            {
                while (field[0] == ' ')
                {
                    field++;
                }
                while (value[0] == ' ')
                {
                    value++;
                }

                // confronta i campi ignorando il case
                if (strcasecmp(field, req->field_codes[i]) == 0 && strcasecmp(value, req->field_values[i]) == 0)
                {
                    // controlla se il nome del campo corrisponde alla richiesta
                    if (strcasecmp(field, req->field_codes[i]) == 0)
                    {
                        // @ temp test
                        printf("\tTRUE\n");
                        subfound = true;
                    }
                }
            }
            token = strtok(NULL, ";");

            // if (pos < (req->size - 1))
            // {
            //     // @ temp test
            //     printf("pos from %d to %d\nchiamata ricorsiva\n", pos, pos + 1);
            //     return recordMatch(pos + 1, record, req);
            // }
            // else
            // {
            //     printf("\tFALSE\n");
            //     return false;
            // }
        }

        found = found&&subfound;
    }
    return found;
}

/*
### Description
    Search a record in the data structure given a keyword to search and a specific field
### Parameters
    bib is a pointer to a BibData data structure
    keyword is what you want to search
    field_code is a code for the field in which you want to search
        (a for author, t for title, e for editor, y for year, n for note,
        c for collocation, d for phisical description, p for pubblication palce)
### Return value
    On research success return a pointer of a Response type variable that contains matched records and how many are them
    On research fail return NULL;
    On fail print error message and and exit
*/
Response *searchRecord(BibData *bib, Request *req)
{
    Response *response = (Response *)malloc(sizeof(Response));
    if (response == NULL)
    {
        // TODO - error handling
    }
    response->size = 0;
    response->records = (char **)malloc(sizeof(char *));
    if (response->records == NULL)
    {
        // TODO - error handling
    }
    int count = 0;

    for (int i = 0; i < bib->size; i++)
    {
        if (recordMatch(bib->book[i], req))
        {
            // @ temp test
            printf("TRUE\n");
            response->records = (char **)realloc(response->records, sizeof(char *) * (response->size + 1));
            response->records[response->size] = (char *)malloc(sizeof(char) * (strlen(bib->book[i]) + 1));
            strcpy(response->records[response->size], bib->book[i]);
            response->size++;
        }
    }

    if (response->size <= 0)
    {
        free(response->records);
        free(response);
        return NULL;
    }

    return response;
}