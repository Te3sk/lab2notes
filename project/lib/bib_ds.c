#include "bib_ds.h"

#define MAX_LENGTH 500
#define MAX_FIELD_LENGTH 30

BibData *createBibData(char *path)
{
    // create a new BibData object
    BibData *bib = malloc(sizeof(BibData));

    // open the file
    FILE *fp = fopen(path, "rb");
    if (fp == NULL)
    {
        // error handling
        perror("fopern failed");
        exit(EXIT_FAILURE);
    }

    // get the number of lines in the file
    char ch;
    bib->size = 0;
    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n')
        {
            bib->size++;
        }
    }

    // go back to the beginning of the file
    rewind(fp);

    // allocate memory for the book array
    bib->book = malloc(bib->size * sizeof(char *));
    if (bib->book == NULL)
    {
        // error handling
        perror("bib->book allocation failed");
        exit(EXIT_FAILURE);
    }

    // allocate memory for each line of the book array
    for (int i = 0; i < bib->size; i++)
    {
        bib->book[i] = malloc(MAX_LENGTH * sizeof(char));
        if (bib->book[i] == NULL)
        {
            // error handling
            perror("bib->book[x] allocation failed");
            exit(EXIT_FAILURE);
        }
    }

    // read the file line by line
    int i = 0;
    while (fgets(bib->book[i], MAX_LENGTH, fp) != NULL)
    {
        // remove the new line character
        bib->book[i][strlen(bib->book[i])] = '\0';
        i++;
    }
    fclose(fp);

    // return the BibData object
    return bib;
}

int* searchRecord(BibData *bib, char *keyword, char field_code)
{
    int *temp_res = (int *)malloc(sizeof(int) * bib->size), count = 0;
    // search for the keyword in each record and return the number of occurrences
    char *field = (char *)malloc(sizeof(char) * MAX_FIELD_LENGTH);
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

    for (int i = 0; i < bib->size; i++)
    {
        char *searchPos = strstr(bib->book[i], keyword);
        if (searchPos != NULL)
        {
            printf("Found at record %d (now count = %d)\n", i, (count+1));
            temp_res[count] = i;
            count++;
        }
    }

    if (count == 0) {
        return NULL;
    } 

    int *res = (int*)malloc(count * sizeof(int));

    for(int i = 0; i < count; i++) {
        res[i] = temp_res[i];
    }

    free(temp_res);

    return res;
}