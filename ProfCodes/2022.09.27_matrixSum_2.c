//Scrivere un programma C che legge da standard input due matrici di interi dimensione r1Xc1 e r2Xc2 e stampa la somma delle due matrici, se possibile (r1==r2 e c1==c2). Il programma deve utilizzare una funzione per calcolare il prodotto. L'input è organizzato come segue: per ogni matrice vengono inseriti prima il numero di righe e colonne, poi i rxc valori interi. Il programma deve eseguire anche il controllo dell'input.
Utilizzare allocazione dinamica e array di puntatori per rappresentare le matrici.

#include <stdlib.h>
#include <stdio.h>

void freeMatrix(int**,int);
int** leggiMatriceDyn(int*, int*);
int** sumDyn(int,int, int**, int**);
void printMatrixDyn(int, int, int**);

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
	int r1,c1,r2,c2, **m1, **m2;

	m1=leggiMatriceDyn(&r1,&c1);
	m2=leggiMatriceDyn(&r2,&c2);

	if (r1==r2 && c1==c2){

		int **m3;
		m3=sumDyn(r1,c1,m1,m2);
		printMatrixDyn(r1,c1,m3);
		freeMatrix(m3,r1);

	} else{
		printf("Le matrici non si possono sommare\n");
	}	

	freeMatrix(m1,r1);
	freeMatrix(m2,r2);

	return 0;
}

void freeMatrix(int** m, int r){
	for(int i=0;i<r;i++)
		free(m[i]);
	free(m);
}


int** allocaMatrice(int r, int c){
	int** risultato;
	risultato=(int**)malloc((r)*sizeof(int*));
        if(risultato==NULL){         
                printf("Memoria esaurita\n");     
                exit(1);
        }
                        
        for(int i=0;i<r;i++){
                risultato[i]=(int*)malloc(c*sizeof(int));
                if(risultato[i]==NULL){
                        printf("Memoria esaurita\n");
                        exit(1);
                }
        }
	return risultato;

}
int** leggiMatriceDyn(int* r, int* c){
	int** risultato;
	*r=readInt();
	*c=readInt();

	risultato=allocaMatrice(*r,*c); 	

	for(int i=0;i<*r;i++)
		for(int j=0;j<*c;j++)
                        risultato[i][j]=readInt();

	
	return risultato;	

}


int** sumDyn(int r,int c, int** m1, int** m2){
	int** somma = allocaMatrice(r,c);
	for(int i=0;i<r;i++)
		for(int j=0;j<c; j++)
			somma[i][j]=m1[i][j]+m2[i][j];
	return somma;
}

void printMatrixDyn(int r, int c, int** m){
	int i,j;
        for(i=0;i<r;i++){
                for (j=0;j<c;j++)
                        printf("%d ",m[i][j]);
                printf("\n");
        }
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








