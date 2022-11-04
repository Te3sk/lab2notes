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
    int controller;

    char a[10], b[10];

    while (!feof(vote))
    {
        fscanf(vote, "%[^,],%[^\n]\n", &a, &b);
        
        check (&cand, a);
        check (&cand, b);
    }

    // int m1 = 0, m2 = 0;

    Cand m1,m2;
    m1.votes = 0;
    m2.votes = 0;

    for (int i = 0; i < cand.length; i++) {
        if (cand.list[i].votes > m1.votes) { //current greater than first max
            m1.votes = cand.list[i].votes;
            // a = cand.list[i].name;
            strcpy(m1.name, cand.list[i].name);
        } else if (cand.list[i].votes < m1.votes) { //current less than first max
            if (cand.list[i].votes > m2.votes) { //current greater than second max
                m2.votes = cand.list[i].votes;
                strcpy(m2.name, cand.list[i].name);
            } else if (cand.list[i].votes == m2.votes) { //current equal to the second max
                m2.votes = cand.list[i].votes;
                strcpy(m2.name, cand.list[i].name); 
            }
        } else if (strcmp(m1.name, cand.list[i].name) < 0) {
            m1.votes = cand.list[i].votes;
            strcpy(m1.name, cand.list[i].name);
        }
    }

    printf("%s con %d voti\n%s con %d voti\n", m1.name, m1.votes, m2.name, m2.votes);
    
}

void check (CandList* cand, char n[]) {
    int c = 0;

    for (int i = 0; i < cand->length; ++i) {
        if (cand->list[i].name == n) {
            cand->list[i].votes++;
            c++;
        }
    }
    if (c == 0) {
        strcpy(cand->list[cand->length].name, n);
        cand->list[cand->length].votes = 1;
        cand->length++;
    }
}