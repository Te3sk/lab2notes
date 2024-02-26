#ifndef LOG_FUNC_H
#define LOG_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LOG_FILE_EXTENSION ".log"

FILE *openLogFile(char *name_bib);

void logQuery(FILE *logFile, char *query, int resultCount);

void logLoan(FILE *logFile, char *record, int resultCount);

void closeLogFile(FILE *logFile);

#endif