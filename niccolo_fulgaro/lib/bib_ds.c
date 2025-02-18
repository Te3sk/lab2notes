#define _GNU_SOURCE

#include "bib_ds.h"

// max length of an entire record
#define MAX_LENGTH 500 // TODO - understand how many bytes give
// max length of a field name
#define MAX_FIELD_LENGTH 30 // TODO - understand how many bytes give
#define THIS_PATH "lib/bib_ds.c/"

/*
### Description
    checks, line by line, if the file is empty and its format
### Parameters
    `path` is the path of the txt file
### Return value
    If the file is totally right (not empty and right format) return the file descriptor
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
        return NULL;
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
    if(bib == NULL){
        // error handling
        perror(THIS_PATH"createBibData - bib allocation failed");
        return NULL;
    }

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
    bib->book = (char **)malloc(sizeof(char *));
    if (bib->book == NULL)
    {
        // error handling
        perror(THIS_PATH "createBibData - bib->book allocation failed");
        fclose(fp);
        // free(bib);
        return NULL;
    }

    char *line = (char *)malloc(MAX_LENGTH * sizeof(char));
    while (fgets(line, MAX_LENGTH, fp) != NULL)
    {
        bib->book[bib->size] = (char *)malloc((strlen(line) + 1) * sizeof(char));
        if (bib->book[bib->size] == NULL)
        {
            // error handling
            perror(THIS_PATH "createBibData - bib->book[bib->size] allocation failed");
            fclose(fp);
            return NULL;
        }

        strcpy(bib->book[bib->size], line);
        bib->size++;

        char **temp = realloc(bib->book, (bib->size + 1) * sizeof(char *));
        if (temp == NULL)
        {
            // error handling
            perror(THIS_PATH "createBibData - temp reallocation failed");
            fclose(fp);
            return NULL;
        }
        bib->book = temp;
    }

    free(line);

    pthread_mutex_init(&(bib->mutex), NULL);

    // close file
    fclose(fp);

    return bib;
}

/*
### Description
    Check if a single record match with a request
### Parameters
    - `char *record` is the single record extract from a bibData
    - `Request *req` is the current request
### Return value
    return `true` if the request match the record, `false` otherwise, NULL on error
*/
bool recordMatch(char *record, Request *req)
{
    bool found = false;
    // make a copy to not modify the original

    char *recordCopy = (char *)malloc((strlen(record) + 1) * sizeof(char));
    if (recordCopy == NULL)
    {
        // error handling
        perror(THIS_PATH "recordMatch - recordCopy allocation failed");
        return NULL;
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
                        char *secRecordCopy = (char *)malloc((strlen(record) + 1) * sizeof(char));
                        if (secRecordCopy == NULL)
                        {
                            // error handling
                            perror(THIS_PATH "recordMatch - recordCopy allocation failed");
                            return NULL;
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
                else
                {
                    return true;
                }
            }
        }

        token = strtok(NULL, ";");
    }
    free(recordCopy);
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
        return NULL;
    }
    response->size = 0;
    response->pos = (int *)malloc(sizeof(int));
    if (response->pos == NULL)
    {
        perror(THIS_PATH "searchRecord - response->pos allocation failed");
        return NULL;
    }
    pthread_mutex_lock(&(bib->mutex));
    for (int i = 0; i < bib->size; i++)
    {
        bool result = recordMatch(bib->book[i], req);
        if (result == true)
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
        response->loan = loanCheck(bib, response->size, response->pos);
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
    - `int N` is the number of record to check
    - `int *rec` is an array with the position of the record to check;
### Return value
    Returne `true` if there isn't the field 'prestito' or if the loan is expired (30 days or more from the date) - THE BOOK CAN BE LOANED
    Returne `false` if the loan isn't expired - THE BOOK CAN'T BE LOANED
*/
bool loanCheck(BibData *bib, int N, int *rec)
{
    // iter for the record that match with the request
    for (int i = 0; i < N; i++)
    {
        // make a copy of the record to not modify the original
        char *recordCopy = (char *)malloc(sizeof(char) * (strlen(bib->book[rec[i]] + 1)));
        strcpy(recordCopy, bib->book[rec[i]]);

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
            strftime(expireDate, 31, ";prestito: %d-%m-%Y %H:%M:%S", tm_now);

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
    if(req->field_codes == NULL){
        // error handling
        perror(THIS_PATH"requestFormatCheck - req->field_codes allocation failed");
        return NULL;
    }
    req->field_values = (char **)malloc(sizeof(char *));
    if(req->field_values == NULL){
        // error handling
        perror(THIS_PATH"requestFormatCheck - req->field_values allocation failed");
        exit(EXIT_FAILURE);
    }

    // copy the request to mantain the original
    char *request_copy = (char *)malloc(sizeof(char) * (strlen(request) + 1));
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
        if (req->field_values[req->size] == NULL)
        {
            // error handling
            perror(THIS_PATH "requestFormatCheck - req->field-values[req->size] allocation failed");
            return NULL;
        }
        strcpy(req->field_values[req->size], value);
        req->field_values[req->size][strlen(req->field_values[req->size])] = '\0';
        // separate code and values in 'token'
        pos[0] = '\0';
        // save codes
        req->field_codes = realloc(req->field_codes, (req->size + 1) * sizeof(char *));
        req->field_codes[req->size] = (char *)malloc(strlen(token) + 1);
        if (req->field_codes[req->size] == NULL)
        {
            // error handling
            perror(THIS_PATH "requestFormatCheck - req->field_codes[req->size] allocation failed");
            return NULL;
        }
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
int updateRecordFile(char *name_bib, char *path, BibData *bib)
{
    // unique path of a temporary file
    char *temp_path = (char *)malloc((strlen("temp_") + strlen(name_bib) + strlen("_record_file.txt")) * sizeof(char));
    sprintf(temp_path, "temp_%s_record_file.txt", name_bib);

    FILE *temp_record = fopen(temp_path, "w");
    if (temp_record == NULL)
    {
        // error handling
        perror(THIS_PATH "updateRecordFile - opening of temp record file failed");
        return -1;
    }

    // TODO - loan check non va bene per questa cosa, fare altra funzione o scrivere qui
    for (int i = 0; i < bib->size; i++) {
        char *recordCopy = (char*)malloc((strlen(bib->book[i]) + 1) * sizeof(char));
        if(recordCopy == NULL){
            // error handling
            perror(THIS_PATH"updateFileRecord - recordCopy allocation failed");
            return -1;
        }
        strcpy(recordCopy, bib->book[i]);

        char *pos = strstr(recordCopy, "prestito:");
        if (pos != NULL){
            pos += strlen("prestito:");
            while(isspace(pos[0])) {
                pos++;
            }
            pos[19] = '\0';

            time_t now;
            now = time(NULL);

            struct tm tm_data;
            memset(&tm_data, 0, sizeof(struct tm));
            strptime(pos, "%d-%m-%Y %H:%M:%S", &tm_data);

            time_t time_data = mktime(&tm_data);
            double diff = difftime(time_data, now);

            if (diff <= 0) {
                pos -= strlen("prestito: ");
                memmove(pos, pos + 30, 30);

                strcpy(bib->book[i], recordCopy);
            }
        }

        if(fputs(bib->book[i], temp_record) == EOF){
            // error handling
            perror(THIS_PATH"updateRecordFile - writing line to file failed");
            return -1;
        }
    }

    rewind(temp_record);
    FILE *original_bib = fopen(path, "w");
    char c;
    while ((c = fgetc(temp_record)) != EOF) {
        fputc(c, original_bib);
    }

    fclose(temp_record);

    remove(temp_path);

    return 1;
}

/*
### Description
    Deallocate all the bib datastructure
### Parameters
    - `BibData *bib` is the data structure to deallocate
*/
void freeBib(BibData *bib) {
    if (bib == NULL) {
        return;
    }

    if (bib->book != NULL) {
        for (int i = 0; i < bib->size; i++) {
            free(bib->book[i]);
        }
        free(bib->book);
        bib->book = NULL;
    }

    pthread_mutex_destroy(&(bib->mutex));

    free(bib);
}
