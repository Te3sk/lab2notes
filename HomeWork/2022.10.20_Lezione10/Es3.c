#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define row_length 100

int main () {
    FILE* bioinfo = fopen("Es3_input.txt", "r");

    if (!bioinfo) {
        perror("Errore nell'apertura del file: ");
        exit(1);
    }

    char line[row_length];
    int number = 0, length = 0, totnuc = 0, prec, max_length = 0;

    while(!feof(bioinfo)) {
        fscanf(bioinfo, "%[^\n]\n", line);

        if (line[0] == '>') {
            prec = 0;
            number++;
            length = 0;
        } else {
            for (int i = 0; i < row_length; i++){
                if (line[i] == 'A' || line[i] == 'C' || line[i] == 'G' || line[i] == 'T') {
                    length++;
                    totnuc++;
                }
            }

            if (max_length < length) {
                max_length = length;
            }
        }

    }

    printf("Numero sequenze: %d\n", number);
    printf("Lunghezza della sequenza più lunga: %d\n", max_length);
    printf("Numero totale di nucleotidi: %d\n", totnuc);
}