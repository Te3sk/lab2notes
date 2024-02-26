#include "log_func.h"

/*
### Description
    opens the log file and truncates (overwrites) it if necessary
### Parameters
    - `char *name_bib` is a string containing the name you whant to give to the log file
### Return value
    on success return the file descriptor of the log file
    on fail print an error msg and exit
*/
FILE *openLogFile(char *name_bib)
{
    // Crea il nome del file di log
    char filename[256];
    snprintf(filename, sizeof(filename), "log/%s%s", name_bib, LOG_FILE_EXTENSION);
    // Apre il file in modalità scrittura (troncamento)
    FILE *logFile = fopen(filename, "w"); // Apre il file in modalità scrittura (troncamento)
    if (logFile == NULL)
    {
        // error handling
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }
    return logFile;
}

/*
### Description
    Function to write a MSG_QUERY request in the log file
### Parameters
    - `FILE *logFile` is the file descriptor of the log file where write datas
    - `char *query` is the request you want to write in the file (empty if the req failed)
    - `inte resultCount` is how many result the request give
*/
void logQuery(FILE *logFile, char *query, int resultCount)
{
    // registra la query se il numero di risultati è maggiore di 0
    if (resultCount > 0)
    {
        fprintf(logFile, "%s\n", query);
    }
    // // //registra il numero di risultati
    // // fprintf(logFile, "QUERY %d\n", resultCount);
    // registra il numero di risultati
    fprintf(logFile, "QUERY %d\n", resultCount);
}

/*
### Description
    Function to record a MSG_LOAN type request in the log file
### Parameters
    - `FILE *logFile` is the file descriptor of the log file where write datas
    - `char *record` is the request you want to write in the file (empty if the req failed)
    - `int resultCount`  is how many result the request give
*/
void logLoan(FILE *logFile, char *record, int resultCount)
{
    // registra il risultato della richiesta
    if (resultCount > 0)
    {
        fprintf(logFile, "LOAN %d\n%s\n", resultCount, record);
    }
    else
    {
        fprintf(logFile, "LOAN 0\n");
    }
}

/*
### Description
    Function to close the log file
### Parameters
    - `FILE *logFile` is the file descriptor of the log file you want to close
*/
void closeLogFile(FILE *logFile)
{
    // Chiude il file di log
    fclose(logFile);
}