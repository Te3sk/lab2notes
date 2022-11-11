#include<stdio.h>
#include<stdlib.h>

#define ndip 4

typedef struct {
    int matr;
    char name[20];
    int mmass;
    int aaass;
    int cash;
} Dipendente;

void compute_cash (Dipendente*);

int main () {
    FILE* reg = fopen("Es4_input2.dat", "rb");

    if (!reg) {
        perror("Error: ");
        exit(1);
    }

    Dipendente dip[4];
    int c = 0;
    char buffer[100];

    while(fread(&dip[c], sizeof(Dipendente), 1, reg)) {
        c++;
    }

    printf("%-4s %-10s %-2s %-4s %-4s\n", "Matr", "Nome", "MM", "AA", "Stipendio");
    for (int i = 0; i < ndip; i++) {
        printf("%-4d %-10s %-2d %-4d %-4d\n", dip[i].matr, dip[i].name, dip[i].mmass, dip[i].aaass, dip[i].cash);
    }

    printf("%-4s %-10s %-4s\n", "Matr", "Nome", "Stipendio");
    compute_cash(&dip[0]);

    return 0;
}

void compute_cash(Dipendente* dip) {
    for (int i = 0; i < ndip; ++i) {
        if (dip[i].aaass < 2000) {
            dip[i].cash = dip[i].cash + ((dip[i].cash * 22) / 100);
            printf("%-4d %-10s %-4d\n", dip[i].matr, dip[i].name, dip[i].cash);
        } else if(dip[i].aaass == 2000 && dip[i].mmass <= 5) {
            dip[i].cash = dip[i].cash + ((dip[i].cash * 22) / 100);
            printf("%-4d %-10s %-4d\n", dip[i].matr, dip[i].name, dip[i].cash);
        }
    }
}