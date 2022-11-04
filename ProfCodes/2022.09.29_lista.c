#include <stdio.h>
#include <stdlib.h>

typedef struct n {

	int val;
	struct n * next;
} Nodo;

typedef Nodo* Lista;

 
void print(Lista );

void insert(int, Lista*);

void insertRic(int, Lista*);

int main(){

	Lista l=NULL;
	int x;
	do{
		scanf("%d",&x);
		insertRic(x,&l);
	}while(x>=0); 
	
	print(l);
}


void insertRic(int x, Lista* l){
	
	if((*l)==NULL || (*l)->val>x){
		Nodo* nuovo=(Nodo*)malloc(sizeof(Nodo));
	
		if(nuovo==NULL){
			printf("Fatal error");
			exit(1);
		}
		nuovo->val=x;
		nuovo->next=*l;
	
		*l=nuovo;
	} else {
		insertRic(x,&((*l)->next));
	}
}

void print(Lista p){
	if(p==NULL)
		return;
	printf("%d\n",p->val);
	print(p->next);

}

void insert(int x, Lista* l){

	Nodo* prec, *succ, *nuovo;
	
	nuovo=(Nodo*)malloc(sizeof(Nodo));
	
	if(nuovo==NULL){
		printf("Fatal error");
		exit(1);
	}
	nuovo->val=x;
	nuovo->next=NULL;

	prec=NULL;
	succ=*l;

	while(succ!=NULL && succ->val<x){
		prec=succ;
		succ=succ->next;
	}

	nuovo->next=succ;
	if(prec!=NULL)
		prec->next=nuovo;
	else
		*l=nuovo;
}




