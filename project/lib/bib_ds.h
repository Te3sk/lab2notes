#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/*
    ### Field
    - `char **book`: pointer to character pointers -> pointer to string array. It contains records (1 for position)
    - `int size`: records number in the varaible above
*/
typedef struct {
    char **book;
    int size;
}BibData;

BibData *createBibData(char *path);

int *searchRecord(BibData *bib, char *keyword, char field_code);

FILE *fileFormatCheck(char *path);