#include <stdio.h>
#include <stdlib.h>

#define N 10


typedef struct
{
    char name[10];
    char surname[10];
    int vote;
} Studente;

void printIfPassed (Studente);

int main() 
{
    Studente stud;
    int count = 0;
    FILE *exam = fopen("Es1_input.txt", "r");
    if (!exam) {
        perror("File opening failed: ");
        exit(EXIT_FAILURE);
    }

    while (fscanf(exam, "%[^;];%[^;];%d\n", &stud.surname, &stud.name, &stud.vote) == 3)
    {
        count++;
        printIfPassed(stud); 
    }
    fclose(exam);

}

void printIfPassed (Studente stud) {
    if (stud.vote >= 18) {
        printf("%10s %10s %4d ESAME SUPERATO\n", stud.surname, stud.name, stud.vote);
    }
    
}