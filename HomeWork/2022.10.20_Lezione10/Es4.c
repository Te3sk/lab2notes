#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define row 4
typedef struct
{
    int matr;
    char name[20];
    int mmass;
    int aaass;
    int cash;
} Dipendente;

void compute_cash();

int main()
{
    FILE *reg = fopen("Es4_input.dat", "rb");
    char *buffer;
    int c, size = fseek(reg, 0, SEEK_END) - fseek(reg, 0, SEEK_SET);
    char temp;

    for (int i = 0; i < 4; i++)
    {
        fread(buffer, sizeof(char), size / 4, reg);
        printf("riga %d -> %s\n", i + 1, buffer);
        fseek(reg, ((size / 4) * i), SEEK_SET);
    }

    printf("\nlunghezza file --> %d\n", c);
}