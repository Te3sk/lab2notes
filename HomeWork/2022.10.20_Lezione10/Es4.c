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
    
    fseek(reg,0, SEEK_END);
    int size = ftell(reg);
    printf("%d\n", size);
    rewind(reg);

    Dipendente *dip = (Dipendente *)malloc(sizeof(dip)*sizeof(reg));
    int *line = (int*)malloc(sizeof(int)*sizeof(reg));
    char c;
    char* buffer = (char*)malloc(200 * sizeof(char));

    for (int i = 0;i< 4; i++) {
        // fscanf(reg, "%s\n", buffer);
        fread(buffer, sizeof(char), 25, reg);
        printf("%s\n", buffer);
    }
}