#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define voteMax 16
#define nameLength 10

typedef struct {
    char name[nameLength];
    int votes;
}Cand;

int main () {
    FILE* vote = fopen("Es2_input.txt", "r");

    char votes_list[voteMax][nameLength];
    int counter = 0;

    if (!vote) {
        perror("Errore nell'apertura del file: ");
        exit(EXIT_FAILURE);
    }

    while (!feof(vote)) {
        fscanf(vote, "%[^,],%[^\n]\n", &votes_list[counter], &votes_list[counter + 1]);
        counter += 2;
    }

    Cand m1, m2;
    m1.votes = -1;
    m2.votes = -1;
    int c;
    for (int i = 0; i < counter -1; i++) {
        c = 1;
        for (int j = i+1; j < counter; j++) {
            if (strcmp(votes_list[i], votes_list[j]) == 0) {
                c++;
            }
        }

        if (c > m1.votes || (c == m1.votes && strcmp(m1.name, votes_list[i]) < 0)) {
            strcpy(m1.name, votes_list[i]);
            m1.votes = c;
        } else if (c < m1.votes) {
            if (c > m2.votes || (c == m2.votes && strcmp(m2.name, votes_list[i]) > 0)) {
                strcpy(m2.name, votes_list[i]);
                m2.votes = c;
            }
        }
    }

    printf("%s con %d voti\n%s con %d voti\n", m1.name, m1.votes, m2.name, m2.votes);

    fclose(vote);
}