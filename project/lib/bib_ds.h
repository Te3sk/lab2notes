#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char **book;
    int size;
}BibData;

BibData * createBibData(char *path);

int* searchRecord(BibData *bib, char *keyword, char field_code);