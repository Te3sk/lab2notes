#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define candNum 20

typedef struct {
    char name[10];
    int votes;
} Cand;

typedef struct {
    Cand list[candNum];
    int length;
}CandList;

void check (CandList*, char[]);

int main () {
    FILE* vote = fopen("Es2_input.txt", "r");

    if (!vote) {
        perror("Errore nell'apertura del file: ");
        exit(EXIT_FAILURE);
    }
    CandList cand;
    cand.length = 0;
    int ca, cb;

    char a[10], b[10];

    while (!feof(vote)) //read from file and fill cand struct
    {
        fscanf(vote, "%[^,],%[^\n]\n", &a, &b);
        
        ca = 0;
        cb = 0;
        
        for (int i = 0; i < cand.length; i++) {
            if (strcmp(a, cand.list[i].name) == 0) {
                cand.list[i].votes++;
                ca++;
            }
        }

        if (ca == 0) {
            strcpy(cand.list[cand.length].name, a);
            cand.list[cand.length].votes = 1;
            cand.length++;
        }

        for (int i = 0; i < cand.length; i++) {
            if (strcmp(b, cand.list[i].name) == 0) {
                cand.list[i].votes++;
                cb++;
            }
        }

        if (cb == 0) {
            strcpy(cand.list[cand.length].name, b);
            cand.list[cand.length].votes = 1;
            cand.length++;
        }

        // check (&cand, a);
        // check (&cand, b);
    }

    Cand m1,m2;
    m1.votes = -1;
    m2.votes = -1;

    for (int i = 0; i < cand.length; i++) { //find 2 max
        if (cand.list[i].votes > m1.votes) {
            //current greater than FIRST max
            m1.votes = cand.list[i].votes;
            strcpy(m1.name, cand.list[i].name);
        } else if (cand.list[i].votes < m1.votes) {
            //current less than FIRST max
            if (cand.list[i].votes > m2.votes) {
                //current greater than SECOND max
                m2.votes = cand.list[i].votes;
                strcpy(m2.name, cand.list[i].name);
            } else if (cand.list[i].votes == m2.votes) {
                //current equal to the SECOND max
                if (strcmp(m2.name, cand.list[i].name) < 0) {
                    //current lexical greater than the SECOND max
                    m2.votes = cand.list[i].votes;
                    strcpy(m2.name, cand.list[i].name);
                }
            }
        } else {
            //current equeal to te FIRST max
            if (strcmp(m1.name, cand.list[i].name) < 0) {
                //current lexical greater than the SECOND max
                m1.votes = cand.list[i].votes;
                strcpy(m1.name, cand.list[i].name);
            }
        }
    }

    printf("%s con %d voti\n%s con %d voti\n", m1.name, m1.votes, m2.name, m2.votes);

    fclose(vote);   
}