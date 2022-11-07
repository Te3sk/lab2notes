#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define row 4
#define buffer_size 1000
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
    unsigned char buffer[buffer_size];

    int file_size = fread(buffer, sizeof(unsigned char), buffer_size, reg);

    printf("file size -> %d bytes\n", file_size);
    printf("size of each element -> %d bytes\n", sizeof(unsigned char));

    for (int i = 0; i < file_size; i++)
    {
        printf("%c", buffer[i]);
    }

    fclose(reg);
}