//Scrivere un programma C che legge da standard input due matrici di interi dimensione r1Xc1 e r2Xc2 e stampa la somma delle due matrici, se possibile (r1==r2 e c1==c2). Il programma deve utilizzare una funzione per calcolare il prodotto. L'input è organizzato come segue: per ogni matrice vengono inseriti prima il numero di righe e colonne, poi i rxc valori interi. Il programma deve eseguire anche il controllo dell'input.


#include <stdlib.h>
#include <stdio.h>

void leggiMatrice(int c, int r, int m[][c]);
void printMatrix(int c, int r, int m[][c]);
void sum(int c, int r, int x[][c],int y[][c],int z[][c]);

int readInt(){
	int x;
	while (scanf("%d",&x)==0){
		printf("Errore in input, inserici un intero valido\n");
		scanf("%*[^\n]\n");
	}
	return x;
}

int main(){

	//leggi matrici
	int r1,c1,r2,c2;

	r1=readInt();	
	c1=readInt();
	int m1[r1][c1];

	leggiMatrice(r1,c1,m1);

	r2=readInt();	
	c2=readInt();
	int m2[r2][c2];

	leggiMatrice(r2,c2,m2);
	
	if (r1==r2 && c1==c2){

		int m3[r1][c1];
		sum(r1,c1,m1,m2,m3);
		printMatrix(r1,c1,m3);

	} else{
		printf("Le matrici non si possono sommare\n");
	}	

	return 0;
}



void leggiMatrice(int c, int r, int m[][c]){
	int i,j;
	for(i=0;i<r;i++)
		for (j=0;j<c;j++)
			m[i][j]=readInt();

}
void printMatrix(int c, int r, int m[][c]){
	int i,j;
	for(i=0;i<r;i++){
		for (j=0;j<c;j++)
			printf("%d ",m[i][j]);
		printf("\n");
	}
}

void sum(int c, int r, int x[][c],int y[][c],int z[][c]){
	int i,j;
	for(i=0;i<r;i++)
		for (j=0;j<c;j++)
			z[i][j]=x[i][j]+y[i][j];
}








