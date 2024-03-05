#ifndef BIB_DS_H
#define BIB_DS_H
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>


#include "pars.h"

/*
    ### Field
    - `char **book`: pointer to character pointers -> pointer to string array. It contains records (1 for position)
    - `int size`: records number in the varaible above
*/
typedef struct
{
    char **book;
    int size;
    pthread_mutex_t mutex;
} BibData;

BibData *createBibData(char *path);

bool recordMatch(char *record, Request *req);

Response *searchRecord(BibData *bib, Request *req);

FILE *fileFormatCheck(char *path);

// bool loanCheck(BibData *bib, Response *response);
bool loanCheck(BibData *bib, int N, int *rec);

void loanUpdate(BibData *bib, Response *response);

void updateDate(struct tm *date, int days);

Request *requestFormatCheck(char *request, char type, int senderFD);

int updateRecordFile(char *name_bib, char *path, BibData *bib);

void freeBib(BibData *bib);

#endif