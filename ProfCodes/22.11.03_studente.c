0 #include<stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 100
#define M 20

	typedef struct
{

	char nome[M];
	char cognome[M];
	int voto;

} Studente;

void printIfPassed(Studente s)
{
	if (s.voto > 17)
		printf("%10s %10s %4d ESAME SUPERATO\n", s.cognome, s.nome, s.voto);
}

int main()
{

	FILE *in = fopen("input.txt", "r");
	if (in)
	{
		char riga[N], *token;
		Studente s;

		while (fgets(riga, N - 1, in))
		{
			// tokeniser
			token = strtok(riga, ";");
			strcpy(s.cognome, token);
			token = strtok(NULL, ";");
			strcpy(s.nome, token);
			token = strtok(NULL, ";");
			s.voto = atoi(token);
			printIfPassed(s);
		}
	}
	else
	{
		perror("Apertura file: ");
		exit(1);
	}

	return 0;
}
