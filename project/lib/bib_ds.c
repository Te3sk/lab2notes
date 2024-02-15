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
    check if a single record match with a request
### Parameters
    - `char *record` is the single record extract from a bibData
    - `Request *req` is the current request
### Return value
    return `true` if the request match the record, `false` otherwise
*/
bool recordMatch(char *record, Request *req)
{
    // @ temp test
    printf("\tENTER IN RECORD MATCH - %s\n", record);
    for (int i = 0; i < req->size; i++)
    {
        // search the field_code in the record
        char *campo = strstr(record, req->field_codes[i]);
        if (campo == NULL)
        {
            // field_code not present in the record
            return false;
        }
        // @ temp test
        printf("\t\t\tcampo: %s\n", campo);
        // take the corrispondent value in the record
        char *valore = strchr(campo, ':');
        if (valore == NULL)
        {
            printf("%srecordMatch - wrong format of the record");
            exit(EXIT_FAILURE);
        }
        // surpasses ':' char
        valore++;
        // @ temp test
        printf("\t\t\tvalore:%s\t\t\tvalue: %s\n", valore, req->field_values[i]);

        // @ temp test
        printf("\nis %s ?= %s\n\n", valore, req->field_values[i]);
        // check if the value founded is equal to the searched
        if (strcmp(valore, req->field_values[i]) != 0)
        {
            // @ temp test
            printf("\t\tFALSE\n");
            return false;
        }
    }
    // @ temp test
    printf("\t\tTRUE\n");
    return true;
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
    // @ temp test
    printf("ENTER IN SEARCH RECORD\n\t");

    char **match = (char **)malloc(sizeof(char *));
    if (match == NULL)
    {
        // error handling
        perror(THIS_PATH "searchRecord - match allocation failed");
        exit(EXIT_FAILURE);
    }
    int count = 0;

    for (int i = 0; i < bib->size; i++)
    {
        if (recordMatch(bib->book[i], req))
        {
            // @ temp test
            printf("\tMATCHA\n");
            match = (char **)realloc(match, sizeof(char *) * (count + 1));
            match[count] = bib->book[i];
            count++;
            // @ temp test
            printf("\tnow count = %d\n", count);
        }
    }

    Response *response = (Response *)malloc(sizeof(Response));
    if (count > 0)
    {
        response->size = count;
        response->records = match;
    }
    else
    {
        response = NULL;
    }
    free(match);
    return response;
}