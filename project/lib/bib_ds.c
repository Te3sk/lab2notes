#define _GNU_SOURCE

#include "bib_ds.h"

// max length of an entire record
#define MAX_LENGTH 500      // TODO - understand how many bytes give
// max length of a field name
#define MAX_FIELD_LENGTH 30 // TODO - understand how many bytes give
#define THIS_PATH "lib/bib_ds.c/"

/*
### Description
    checks, line by line, if the file is empty and its format
### Parameters
    `path` is the path of the txt file
### Return value
    If the file is totally right (not empty and right format) return 1
    If the file is empty, doesn't exists or of the wrong format return NULL
*/
FILE *fileFormatCheck(char *path)
{

    // open the file
    FILE *fp = fopen(path, "rb");
    if (fp == NULL)
    {
        return NULL;
    }

    char *buffer = (char *)malloc(sizeof(char) * MAX_LENGTH);
    if (buffer == NULL)
    {
        // error handling
        perror(THIS_PATH "/fileFormatCheck - buffer allocation failed");
        exit(EXIT_FAILURE);
    }

    // read the file line by line
    while (fgets(buffer, MAX_LENGTH, fp) != NULL)
    {
        if (strlen(buffer) == 0)
        {
            // if the line readed is empty
            continue;
        }
        // end sign at the end of the string
        buffer[strlen(buffer) - 1] = '\0';

        // tokenize for ";" char
        char *token = strtok(buffer, ";");
        while (token != NULL)
        {
            // delete spaces char
            while (token[0] == ' ')
            {
                memmove(token, token + 1, strlen(token));
            }

            if (strlen(token) != 0)
            {
                // end sign at the end of the string
                token[strlen(token) - 1] = '\0';

                // check if there aren't ":" char OR if there are "," char and it is before ":"
                if ((strstr(token, ":") == NULL) || ((strchr(token, ',')) && ((strchr(token, ',') - token) < (strchr(token, ':') - token))))
                {
                    printf("wrong file format\n");
                    return NULL;
                }
            }
            token = strtok(NULL, ";");
        }
    }

    free(buffer);

    return fp;
}

/*
### Description
    Creation of data structure give the path of a txt file
### Darameters
    `path` is the path of the txt file
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
    bib->size = 0;
    bib->book = malloc((bib->size + 1) * sizeof(char *));
    if (bib->book == NULL)
    {
        // error handling
        perror(THIS_PATH "/createBibData - bib->book allocation failed");
        exit(EXIT_FAILURE);
    }

    bib->book[bib->size] = (char *)malloc((MAX_LENGTH + 1) * sizeof(char));
    while (fgets(bib->book[bib->size], MAX_LENGTH, fp) != NULL)
    {
        bib->book[bib->size][strlen(bib->book[bib->size]) - 1] = '\0';
        bib->size++;
        bib->book = realloc(bib->book, (bib->size + 1) * sizeof(char *));
        bib->book[bib->size] = (char *)malloc((MAX_LENGTH + 1) * sizeof(char));
    }

    pthread_mutex_init(&(bib->mutex), NULL);

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
                if (req->size > 1)
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
                            // free(recordCopy);
                            // free(secRecordCopy);
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
                else
                {
                    // free(recordCopy);
                    // free(secRecordCopy);
                    return true;
                }
            }
        }

        token = strtok(NULL, ";");
    }
    // free(recordCopy);
    // free(secRecordCopy);
    return found;
}

/*
### Description
    Search a record in the data structure given a keyword to search and a specific field
### Parameters
    `bib` is a pointer to a BibData data structure
    `keyword` is what you want to search
    `field_code` is a code for the field in which you want to search
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
        perror(THIS_PATH "searchRecord - response allocation failed");
        exit(EXIT_FAILURE);
    }
    response->size = 0;
    response->pos = (int *)malloc(sizeof(int));
    if (response->pos == NULL)
    {
        perror(THIS_PATH "searchRecord - response->pos allocation failed");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&(bib->mutex));
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
        response->loan = loanCheck(bib, response);
        if (response->loan)
        {
            loanUpdate(bib, response);
        }
    }

    pthread_mutex_unlock(&(bib->mutex));

    return response;
}

/*
### Description
    Check if the loan if aviable checking check if the field 'prestito' is there and comparing it with te actual date if necessary
### Parameters
    - `BibData *bib` is the pointer to the bib datastructure (all records)
    - `Response *response` is the datastructure with the response for answare to the client
### Return value
    Returne `true` if there isn't the field 'prestito' or if the loan is expired (30 days or more from the date) - THE BOOK CAN BE LOANED
    Returne `false` if the loan isn't expired - THE BOOK CAN'T BE LOANED
*/
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

            time_t now;
            now = time(NULL);

            // parse the string
            struct tm tm_data;
            memset(&tm_data, 0, sizeof(struct tm));
            strptime(pos, "%d-%m-%Y %H:%M:%S", &tm_data);

            // add 30 days
            updateDate(&tm_data, 30);

            free(recordCopy);

            // comparation
            time_t time_data = mktime(&tm_data);
            double diff = difftime(time_data, now);

            if (diff <= 0)
            {
                // time_data <= now
                return true;
            }
            else
            {
                // time_data > now
                return false;
            }
        }
        else
        {
            return true;
        }
    }
    return true;
}

/*
### Description
    Update the loan expire date in the record
### Parameters
    - `BibData *bib` is the data structure containing the records
    - `Response *response` is the data structure containing the positions of the focus records in bib
*/
void loanUpdate(BibData *bib, Response *response)
{
    for (int i = 0; i < response->size; i++)
    {
        char *pos = strstr(bib->book[response->pos[i]], "prestito");
        if (pos != NULL)
        {

            char *expireDate = (char *)malloc(sizeof(char) * 29);
            if (expireDate == NULL)
            {
                // error handling
                perror(THIS_PATH "loanUpdate - expireDate allocation failed");
                exit(EXIT_FAILURE);
            }
            // get actual time
            time_t now = time(NULL);
            struct tm *tm_now = localtime(&now);
            // add 30 days
            updateDate(tm_now, 30);
            // foramt the date in a string
            strftime(expireDate, 31, "prestito: %d-%m-%Y %H:%M:%S", tm_now);

            // update the value in the field
            char *pos = strstr(bib->book[response->pos[i]], "prestito:");
            // skip spaces
            while (isspace(pos[0]))
            {
                pos++;
            }

            strncpy(pos, expireDate, strlen(expireDate));
            free(expireDate);
        }
        else
        {

            char *expireDate = (char *)malloc(sizeof(char) * 29);
            if (expireDate == NULL)
            {
                // error handling
                perror(THIS_PATH "loanUpdate - expireDate allocation failed");
                exit(EXIT_FAILURE);
            }
            // get actual time
            time_t now = time(NULL);
            struct tm *tm_now = localtime(&now);
            // add 30 days
            updateDate(tm_now, 30);
            // format the date in a string
            strftime(expireDate, 31, "prestito: %d-%m-%Y %H:%M:%S", tm_now);

            // add the field:value in the record
            bib->book[response->pos[i]] = realloc(bib->book[response->pos[i]], (strlen(bib->book[response->pos[i]]) + strlen(expireDate) + 1));
            bib->book[response->pos[i]][strlen(bib->book[response->pos[i]]) - 1] = '\0';
            strcat(bib->book[response->pos[i]], expireDate);
            free(expireDate);
        }
    }
}

/*
### Description
    add `days` days on a date, updating also month and year
### Parameters
    - `struct tm *date` is the date to update
    - `int days` indicates how many days to increase the date
*/
void updateDate(struct tm *date, int days)
{
    int monthdays;
    if (date->tm_mon == 0 || date->tm_mon == 2 || date->tm_mon == 4 || date->tm_mon == 6 || date->tm_mon == 7 || date->tm_mon == 9 || date->tm_mon == 11)
    {
        monthdays = 31;
    }
    else if (date->tm_mon == 1)
    {
        if (date->tm_year % 4 == 0)
        {
            monthdays = 29;
        }
        else
        {
            monthdays = 28;
        }
    }
    else
    {
        monthdays = 30;
    }

    if (date->tm_mday + days <= monthdays)
    {
        date->tm_mday += days;
    }
    else
    {
        if (date->tm_mon == 11)
        {
            date->tm_year++;
            date->tm_mon = 0;
        }
        else
        {
            date->tm_mon++;
        }
        date->tm_mday = date->tm_mday + days - monthdays;
    }
}

/*
### Description
    Take the string with the request sended by client and put infos in the `Response` data structure
### Parameters
    - `char *request` is the string containing the request by the client
    - `char type` is the char with the type of msg the client send before the msg
    - `int senderFD` is the file descriptor of the client that make the request
### Return value
    On success (request of the right format) return a pointer to the `Request` data structure filled with the infos
    On fail (wrong request format or wrong type char) return null
*/
Request *requestFormatCheck(char *request, char type, int senderFD)
{
    // initialize data structure
    Request *req = (Request *)malloc(sizeof(Request));
    req->senderFD = senderFD;
    req->size = 0;
    if (type == 'L')
    {
        req->loan = true;
    }
    else if (type == 'Q')
    {
        req->loan = false;
    }
    else
    {
        printf(THIS_PATH "requestFormatCheck, ERROR: invalid request type\n");
        return NULL;
    }
    req->field_codes = (char **)malloc(sizeof(char *));
    req->field_values = (char **)malloc(sizeof(char *));

    // copy the request to mantain the original
    char *request_copy = (char *)malloc(strlen(request)); 
    strcpy(request_copy, request);

    // tokenize for ";"
    char *token = strtok(request_copy, ";");
    while (token != NULL)
    {
        // check the format
        char *pos = strstr(token, ":");
        if (pos == NULL)
        {
            printf(THIS_PATH "requestFormatCeck, ERROR: invalid request format\n");
            return NULL;
        }
        // save values
        char *value = pos + 1;
        req->field_values = realloc(req->field_values, (req->size + 1) * sizeof(char *));
        req->field_values[req->size] = (char *)malloc(strlen(value) + 1);
        strcpy(req->field_values[req->size], value);
        req->field_values[req->size][strlen(req->field_values[req->size])] = '\0';
        // separate code and values in 'token'
        pos[0] = '\0';
        // save codes
        req->field_codes = realloc(req->field_codes, (req->size + 1) * sizeof(char *));
        req->field_codes[req->size] = (char *)malloc(strlen(token) + 1);
        strcpy(req->field_codes[req->size], token);
        req->field_codes[req->size][strlen(req->field_codes[req->size])] = '\0';
        token = strtok(NULL, ";");
        req->size++;
    }
    free(request_copy);

    for (int i = 0; i < req->size; i++)
    {
        if (strcmp(req->field_codes[i], "author") == 0)
        {
            req->field_codes[i] = "autore";
        }
        else if (strcmp(req->field_codes[i], "title") == 0)
        {
            req->field_codes[i] = "titolo";
        }
        else if (strcmp(req->field_codes[i], "year") == 0)
        {
            req->field_codes[i] = "anno";
        }
        else if (strcmp(req->field_codes[i], "editor") == 0)
        {
            req->field_codes[i] = "editore";
        }
        else if (strcmp(req->field_codes[i], "collocation") == 0)
        {
            req->field_codes[i] = "collocazione";
        }
        else if (strcmp(req->field_codes[i], "physical_description") == 0)
        {
            req->field_codes[i] = "descrizione_fisica";
        }
    }

    // // free(request_copy);
    return req;
}

/*
### Description
    Update the file_record from bibData datastructure
### Parameters
    - `char *path` is the path of the file_record
    - `BibData *bib` is the datastructure with the info to overwrite
### Return value
    On success return 1
    On fail print an error msg and return -1
*/
int updateRecordFile(char *path, BibData *bib)
{
    FILE *fp = fopen(path, "w");
    if (fp == NULL)
    {
        // error handling
        perror(THIS_PATH "updateRecordFile - fopen failed");
        return -1;
    }

    for (int i = 0; i < bib->size; i++)
    {
        // make a copy to mantain the original string
        char *copy = (char *)malloc(sizeof(char) * strlen(bib->book[i]));
        if (copy == NULL)
        {
            // error handling
            perror(THIS_PATH "updateFileRecord - copy allocation failed");
            free(copy);
            return -1;
        }
        strcpy(copy, bib->book[i]);
        // search for "prestito" field
        char *pos = strcasestr(copy, "prestito:");
        free(copy);
        if (pos != NULL)
        {
            // skip "prestito" and delete initial spaces if there are
            pos += strlen("prestito:");
            while (isspace(pos[0]))
            {
                pos++;
            }
            pos[19] = '\0';

            time_t now;
            now = time(NULL);

            // parse the string
            struct tm tm_data;
            memset(&tm_data, 0, sizeof(struct tm));
            strptime(pos, "%d-%m-%Y %H:%M:%S", &tm_data);

            // comparation
            time_t time_data = mktime(&tm_data);
            double diff = difftime(time_data, now);

            if (diff <= 0)
            {
                // prestito scaduto -> rimuovi il campo prestito
                char *start_prest = strstr(bib->book[i], "prestito:");
                char *end_prest = strstr(bib->book[i], ";");
                // // int remove_length = end_prest - start_prest;
                memmove(start_prest, end_prest, strlen(end_prest));
            }
        }

        // write the record in the file
        fputs(bib->book[i], fp);
    }

    fclose(fp);
    return 1;
}