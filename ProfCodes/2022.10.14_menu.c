#define N 100
#include <stdio.h>
#include <string.h>

char str[N];

void nuovaStringa();
void contaCaratteri();
void contaParole();
void contaRighe();

int main(){

	int opzione=-1;
	void (*f[4])(void)={nuovaStringa, contaCaratteri, contaParole, contaRighe};
	while(1){
		printf("Inserisci l'opzione: 0- nuova stringa, 1- conta caratteri, 2 - conta parole, 3 - conta righe\n");
		scanf("%d",&opzione);
		getchar();
		if(opzione<4 && opzione>=0)
			(*f[opzione])();
		else
			break;
	}
	return 0;
}


void nuovaStringa(){
	
	printf("Inserisci nuova stringa:\n");
	char temp[N]="";
	str[0]='\0';
	while(1){
		fgets(temp, N, stdin);
		if(strcmp(temp,"****\n")==0)
			return;
		strcat(str, temp);
	}
}

void contaCaratteri(){ 
        printf("caratteri: %ld\n",strlen(str));
}

void contaParole(){ 
        int n=0;
	char strcp[strlen(str)+1];
	strcpy(strcp,str);
	char* tok=strtok(strcp," \n");
	if(tok!=NULL)
		n=1;
	while((tok=strtok(NULL," \n"))!=NULL)
		n++;
	printf("parole: %d\n",n);
}

void contaRighe(){
	printf("conta righe");
}
