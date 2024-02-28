#ifndef LOG_FUNC_H
#define LOG_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 

#define LOG_FILE_EXTENSION ".log"

int openLogFile(char *name_bib);

void logQuery(int logFile, char *query, int resultCount);

void logLoan(int logFile, char *record, int resultCount);

#endif