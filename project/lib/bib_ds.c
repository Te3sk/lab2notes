#include "bib_ds.h"

#define MAX_LENGTH 500
#define MAX_FIELD_LENGTH 30
#define THIS_PATH "lib/bib_ds.c/"

FILE *fileFormatCheck(char *path)
{
    /*
    # description
        checks, line by line, if the file is empty and its format
    # parameters
        'path' is the path of the txt file
    # return value
        If the file is totally right (not empty and right format) return 1
        If the file is empty or of the wrong format return NULL
    */

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
    bool notEmpty = false;
    bool rightFormat = false;

    while (fgets(buffer, MAX_LENGTH, fp) != NULL)
    {
        if (strlen(buffer) == 0)
        {
            continue;
        }
        buffer[strlen(buffer) - 1] = '\0';
        notEmpty = true;
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

BibData *createBibData(char *path)
{
    /*
    # description
        Creation of data structure give the path of a txt file
    # parameters
        'path' is the path of the txt file
    # return value
        On success return a pointer to a BibData variable (char** data + int size)
        filled with the data
        if the file is of the wrong format or empty return NULL
    */

    // create a new BibData object
    BibData *bib = malloc(sizeof(BibData));

    // open the file
    // FILE *fp = fopen(path, "rb");
    FILE *fp = fileFormatCheck(path);

    if (fp == NULL)
    {
        // error handling
        printf("wrong file format\n");
        return NULL;
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
        perror(THIS_PATH "/createBibData - bib->book allocation failed");
        exit(EXIT_FAILURE);
    }

    char *buffer = malloc(MAX_LENGTH * sizeof(char));
    int i = 0;

    

    bib->book[0] = (char *)malloc((MAX_LENGTH + 1) * sizeof(char));
    while(fgets(bib->book[0], MAX_LENGTH, fp) != NULL){
        printf("%s\n", bib->book[i]);
        i++;
    }


    // while (fgets(buffer, MAX_LENGTH, fp) != NULL)
    // {
    //     buffer[strlen(buffer) - 1] = '\0'; // remove the newline character
    //     printf("%s\n#######\n", buffer);
    //     bib->book[i] = malloc((strlen(buffer) + 1) * sizeof(char));
    //     strcpy(bib->book[i], buffer);
    //     printf("%s\n------------------------\n", bib->book[i]);
    //     printf("buffer %s bib->book[%d] | buffer size: %d vs bib->book[%d] size: %d\nbuffer endstr in pos: %d vs bib->book[%d] endstr in pos: %d\n", (buffer == bib->book[i] ? "==" : "!="), i, strlen(buffer), i, strlen(bib->book[i]), (strchr(buffer, '\0') - buffer), i, (strchr(bib->book[i], '\0') - bib->book[i]));
    //     printf("size of buffer : %d | size of bib->book[%d] : %d\n", sizeof(buffer), i, sizeof(bib->book[i]));
    //     printf("--------------------------------------------------------------------\n");
    //     // for (int j = 0; j < strlen(buffer); j++){
    //     //     printf("\t\t%c\t|\t%c\t|t\t%c\n", buffer[j], bib->book[i][j], bufcpy[j]);
    //     // }

    //     i++;
    // }

    // // for (int i = 0; i < bib->size; i++)
    // // {
    // //     bib->book[i] = malloc(MAX_LENGTH * sizeof(char));
    // //     if (bib->book[i] == NULL)
    // //     {
    // //         // error handling
    // //         perror(THIS_PATH "/createBibData - bib->book[i] allocation failed");
    // //         exit(EXIT_FAILURE);
    // //     }

    // //     printf("iterazione %d\n\t- ");
    // //     fgets(bib->book[i], MAX_LENGTH, fp);
    // //     printf("%s\n", bib->book[i]);

    // // }

    // char *buffer = malloc(MAX_LENGTH * sizeof(char));

    // bib->book[0] = (char *)malloc(MAX_LENGTH * sizeof(char));

    // fgets(bib->book[0], MAX_LENGTH, fp);
    // printf("%s\n\n");

    // while(fgets(buffer, MAX_LENGTH, fp) != NULL) {
    //     printf("%s\n", buffer);
    // }

    // while (fgets(bib->book[i], MAX_LENGTH, fp) != NULL)
    // {
    //     // remove the new line character
    //     bib->book[i][strlen(bib->book[i])] = '\0';
    //     i++;
    // }

    // // // @ temp test
    // char* buffer = malloc(MAX_LENGTH * sizeof(char));

    // while (fgets(buffer, MAX_LENGTH, fp) != NULL) {
    //     printf("%s\n", buffer);
    // }

    // // read the file line by line
    // int i = 0;
    // while (fgets(buffer, MAX_LENGTH, fp) != NULL)
    // {
    //     // @ temp test
    //     // remove the new line character
    //     buffer[strlen(buffer)-1] = '\0';
    //     printf("######\n%s\n", buffer);
    //     bib->book[i] = (char*)malloc(sizeof(buffer) + 1);
    //     if(bib->book[i] == NULL){
    //         // error handling
    //         perror("lib/bib_ds.c/createBibData - bib->book[i] allocation failed");
    //         exit(EXIT_FAILURE);
    //     }
    //     strcpy(bib->book[i], buffer);
    //     printf("----\n%s\n\n", bib->book[i]);
    //     i++;
    // }
    // fclose(fp);

    // return the BibData object
    return bib;
}

int *searchRecord(BibData *bib, char *keyword, char field_code)
{
    /*
    # description
        Search a record in the data structure given a keyword to search and a specific field
    # parameters
        bib is a pointer to a BibData data structure
        keyword is what you want to search
        field_code is a code for the field in which you want to search
            (a for author, t for title, e for editor, y for year, n for note,
            c for collocation, d for phisical description, p for pubblication palce)
    # return value
        On research success return a pointer of an integer array containing the index
        of found result. The array terminate with -1 (sentinel)
        On research fail return an array with one position equal to -1
        On fail print error message and and return NULL
    */

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