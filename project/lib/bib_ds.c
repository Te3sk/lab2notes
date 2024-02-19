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
    Check if a single record match with a request
### Parameters
    - `char *record` is the single record extract from a bibData
    - `Request *req` is the current request
### Return value
    return `true` if the request match the record, `false` otherwise
*/
bool recordMatch(char *record, Request *req)
{
    bool found = false;
    // make a copy to not modify the original
    char *recordCopy = (char *)malloc(strlen(record) * sizeof(char));
    if (recordCopy == NULL)
    {
        // error handling
        perror(THIS_PATH "recordMatch - recordCopy allocation failed");
        exit(EXIT_FAILURE);
    }
    strcpy(recordCopy, record);
    // tokenize record field
    char *token = strtok(recordCopy, ";");
    while (token != NULL)
    {
        // remove initial spaces
        while (isspace(token[0]))
        {
            token++;
        }
        // check if the field_codes match
        if (strncasecmp(req->field_codes[0], token, strlen(req->field_codes[0])) == 0)
        {
            // skip field_codes, spaces and ':' char
            token += strlen(req->field_codes[0]);
            while (isspace(token[0]) || token[0] == ':')
            {
                token++;
            }
            // check if the field value match
            if (strncasecmp(req->field_values[0], token, strlen(req->field_values[0])) == 0)
            {
                for (int i = 1; i < req->size; i++)
                {
                    // make a copy to not modify the original
                    char *secRecordCopy = (char *)malloc(strlen(record) * sizeof(char));
                    if (secRecordCopy == NULL)
                    {
                        // error handling
                        perror(THIS_PATH "recordMatch - recordCopy allocation failed");
                        exit(EXIT_FAILURE);
                    }
                    strcpy(secRecordCopy, record);
                    // find the next request field in record
                    char *pos = strcasestr(secRecordCopy, req->field_codes[i]);
                    if (pos == NULL)
                    {
                        // not found
                        found = false;
                        break;
                    }
                    else
                    {
                        // found, skip the field_code, spaces and ':' char
                        pos += strlen(req->field_codes[i]);
                        while (isspace(pos[0]) || pos[0] == ':')
                        {
                            pos++;
                        }
                        // compare with the field_value
                        if (strncasecmp(pos, req->field_values[i], strlen(req->field_values[i])) == 0)
                        {
                            found = true;
                        }
                        else
                        {
                            found = false;
                        }
                    }
                }
            }
        }

        token = strtok(NULL, ";");
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
    response->pos = (int *)malloc(sizeof(int));
    if (response->pos == NULL)
    {
        // TODO - error handling
    }
    int count = 0;

    for (int i = 0; i < bib->size; i++)
    {
        if (recordMatch(bib->book[i], req))
        {
            response->pos = (int *)realloc(response->pos, sizeof(int) * (response->size + 1));
            response->pos[response->size] = i;
            response->size++;
        }
    }

    if (response->size <= 0)
    {
        free(response->pos);
        free(response);
        return NULL;
    }

    if (req->loan)
    {
        // @ temp test
        printf("\nrichiesto prestito\n\n");
        bool temp = loanCheck(bib, response);
        printf("\tloan : %d\n\n", temp);
    }

    return response;
}

bool loanCheck(BibData *bib, Response *response)
{
    // iter for the record that match with the request
    for (int i = 0; i < response->size; i++)
    {
        // make a copy of the record to not modify the original
        char *recordCopy = (char *)malloc(sizeof(char) * (strlen(bib->book[response->pos[i]]) + 1));
        strcpy(recordCopy, bib->book[response->pos[i]]);

        char *pos = strcasestr(recordCopy, "prestito:");
        if (pos != NULL)
        {
            // skip field_code and spaces and set the end of the string
            pos += strlen("prestito:");
            while (isspace(pos[0]))
            {
                pos++;
            }
            pos[19] = '\0';
            // separate date from hour
            char *date = pos;
            char *hour = pos + 11;
            date[10] = '\0';
            // save individually all datas
            int y, m, d, h, min, sec;
            char *dtoken = strtok(date, "-");
            d = atoi(dtoken);
            dtoken = strtok(NULL, "-");
            m = atoi(dtoken);
            dtoken = strtok(NULL, "-");
            y = atoi(dtoken);
            char *htoken = strtok(hour, ":");
            h = atoi(htoken);
            htoken = strtok(NULL, ":");
            min = atoi(htoken);
            htoken = strtok(NULL, ":");
            sec = atoi(htoken);
            // get the actual time
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            // TODO - continue...
        }
        else
        {
            // @ temp test
            printf("\tprestito disponibile\n");
            return true;
        }
    }
}