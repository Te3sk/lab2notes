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
    Search a record in the data structure given a keyword to search and a specific field
### Parameters
    bib is a pointer to a BibData data structure
    keyword is what you want to search
    field_code is a code for the field in which you want to search
        (a for author, t for title, e for editor, y for year, n for note,
        c for collocation, d for phisical description, p for pubblication palce)
### Return value
    On research success return a pointer of an integer array containing the index
    of found result. The array terminate with -1 (sentinel)
    On research fail return an array with one position equal to -1
    On fail print error message and and return NULL
*/
int *searchRecord(BibData *bib, char *keyword, char field_code)
{

    // create a temporary array to store the results
    int *temp_res = (int *)malloc(sizeof(int) * bib->size), count = 0;
    if (temp_res == NULL)
    {
        // error handling
        perror("lib/bib_ds.c/searchRecord - temp_res failed");
        return NULL;
    }

    // search for the keyword in each record and return the number of occurrences
    char *field = (char *)malloc(sizeof(char) * MAX_FIELD_LENGTH);
    if (field == NULL)
    {
        // error handling
        perror("lib/bib_ds.c/searchRecord - field allocation failed");
        return NULL;
    }

    switch (field_code)
    {
    case 'a':
        strcpy(field, "autore");
        break;
    case 't':
        strcpy(field, "titolo");
        break;

    case 'e':
        strcpy(field, "editore");
        break;

    case 'y':
        strcpy(field, "anno");
        break;

    case 'n':
        strcpy(field, "nota");
        break;

    case 'c':
        strcpy(field, "collocazione");
        break;

    case 'p':
        strcpy(field, "luogo_pubblicazione");
        break;

    case 'd':
        strcpy(field, "descrizione_fisica");
        break;

    default:
        printf("Invalid field code\n");
        return NULL;
    }

    // search for the keyword in each record and return the number of occurrences
    for (int i = 0; i < bib->size; i++)
    {
        char *searchPos = strstr(bib->book[i], keyword);
        if (searchPos != NULL)
        {
            temp_res[count] = i;
            count++;
        }
    }

    int *res;

    if (count == 0)
    {
        printf("No match in data set with %s : %s\n", field, keyword);
        res = (int *)malloc(sizeof(int));
        if (res == NULL)
        {
            // error handling
            perror("lib/bib_ds.c/searchRecord - res allocation failed");
            exit(EXIT_FAILURE);
        }
        res[0] = -1;
    }
    else
    {
        // allocate memory for the result array
        res = (int *)malloc((count + 1) * sizeof(int));

        // copy the results from the temporary array to the result array
        for (int i = 0; i < count; i++)
        {
            res[i] = temp_res[i];
        }

        res[count] = -1; // sentinel
    }

    // free the temporary array
    free(temp_res);
    // free the field string
    free(field);
    return res;
}

// int *requestFormatCheck(char *request, char *format)
// {
//     // TODO - decide what format a request must have
// }