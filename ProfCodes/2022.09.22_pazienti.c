//Scrivere un programma che gestisce pazienti in un ospedale. I pazienti sono identificati da un id numerico intero. Possono essere o pazienti con diabete (1), o pazienti con malattie cardiovasculari (0). Per ogni tipo di paziente sono disponibili diversi di variabili cliniche. Per i pazienti con diabete abbiamo la glicemia (float) e il peso (float), per i pazienti cardio abbiamo pressione (int) e frequenza cardiaca (int). Il programma legge un numero n da tastiera, seguito da n pazienti. Per ogni paziente abbiamo l'ID e il tipo di malattia sulla prima riga, e le due variabili di interesse sulla seconda riga. Il programma stampa tutti i pazienti che hanno la stessa malattia dell'ultimo paziente inserito.

#include <stdio.h>
typedef struct{
	float glicemia;
	float peso;
} ValDiabete;

typedef struct{
	int pressione;
	int fc;
} ValCardio;

typedef union{
	ValCardio vc;
	ValDiabete vd;
	
} Valori;

typedef struct{
	int id;
	int diagnosi;
	Valori val; 
} Paziente;

int main(){
	int n;
	scanf("%d",&n);
	Paziente lista[n];

	
	for (int i=0;i<n;i++){
		scanf("%d %d",&lista[i].id,&lista[i].diagnosi);
		if(lista[i].diagnosi==0){
			scanf("%d %d",&lista[i].val.vc.fc,&lista[i].val.vc.pressione);
		} else {
			scanf("%f %f",&lista[i].val.vd.glicemia,&lista[i].val.vd.peso);

		}
	}

	for (int i=0;i<n;i++){
		if(lista[i].diagnosi==lista[n-1].diagnosi){
			if(lista[i].diagnosi==0)
				printf("ID:%d, FC:%d, Pressione:%d",lista[i].id, lista[i].val.vc.fc,lista[i].val.vc.pressione);
			else
				printf("ID:%d, Glicemia:%f, Peso:%f",lista[i].id, lista[i].val.vd.glicemia,lista[i].val.vd.peso);
				
		}
	}
	
	
}







