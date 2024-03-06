#include "log_func.h"

#define THIS_PATH "lib/log_func.c/"

/*
### Description
    opens the log file and truncates (overwrites) it if necessary
### Parameters
    - `char *name_bib` is a string containing the name you whant to give to the log file
### Return value
    on success return the file descriptor of the log file
    on fail print an error msg and exit
*/
int openLogFile(char *name_bib)
{
    // Crea il nome del file di log
    char filename[256];
    snprintf(filename, sizeof(filename), "log/%s%s", name_bib, LOG_FILE_EXTENSION);

    int logFD = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (logFD == -1)
    {
        // error handling
        perror(THIS_PATH "openLogFile - file opening failed");
        exit(EXIT_FAILURE);
    }

    return logFD;
}

/*
### Description
    Function to write a MSG_QUERY request in the log file
### Parameters
    - `FILE *logFile` is the file descriptor of the log file where write datas
    - `char *query` is the request you want to write in the file (empty if the req failed)
    - `inte resultCount` is how many result the request give
*/
void logQuery(int logFile, char *record, int resultCount)
{
    // registra la query se il numero di risultati è maggiore di 0
    char *data;
    if (resultCount > 0)
    {
        data = (char *)malloc((strlen(record) + strlen("QUERY ") + 4) * sizeof(char));
        sprintf(data, "QUERY %d\n%s\n", resultCount, record);
        write(logFile, data, strlen(data));
    }
    else
    {
        data = (char *)malloc((strlen("QUERY 0\n") +1) * sizeof(char));
        strcpy(data, "QUERY 0\n");
        write(logFile, data, strlen(data));
    }
}

/*
### Description
    Function to record a MSG_LOAN type request in the log file
### Parameters
    - `FILE *logFile` is the file descriptor of the log file where write datas
    - `char *record` is the request you want to write in the file (empty if the req failed)
    - `int resultCount`  is how many result the request give
*/
void logLoan(int logFile, char *record, int resultCount)
{
    // registra il risultato della richiesta
    char *data;
    if (resultCount > 0)
    {
        data = (char *)malloc((strlen(record) + strlen("LOAN ") + 4) * sizeof(char));
        sprintf(data, "LOAN %d\n%s\n", resultCount, record);
        write(logFile, data, strlen(data));
        // fprintf(logFile, "LOAN %d\n%s\n", resultCount, record);
    }
    else
    {
        data = (char *)malloc((strlen("LOAN 0") + 2) * sizeof(char));
        strcpy(data, "LOAN 0\n");
        write(logFile, data, strlen(data));
        // fprintf(logFile, "LOAN 0\n");
    }
}
