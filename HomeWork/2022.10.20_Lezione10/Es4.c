#include<stdio.h>
#include<stdlib.h>

typedef struct {
    int matr;
    char name[20];
    int mmass;
    int aaass;
    int cash;
} Dipendente;

void compute_cash ();

int main () {
    FILE* reg = fopen("Es4_input.dat", "rb");

    if (!reg) {
        perror("Error: ");
        exit(1);
    }

    Dipendente dip[4];
    int c = 0;
    char buffer[200];

    for (int i = 0;i< 4; i++) {
        // fscanf(reg, "%d %s %d %d %d\n", &dip[c].matr, &dip[c].name, &dip[c].mmass, &dip[c].aaass, &dip[c].cash);
        // printf("-> %d %s %d %d %d\n", dip[c].matr, dip[c].name, dip[c].mmass, dip[c].aaass, dip[c].cash);
        // c++;
        fscanf(reg, "%[^\n]\n", &buffer);
        printf("%s\n", buffer);
    }
}