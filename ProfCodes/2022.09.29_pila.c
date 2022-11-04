// PILA - LIFO - 
// push
// pop 
// peek
// size
// print

#include <stdio.h>
#include <stdlib.h>


typedef struct n {
	int val;
	struct n * next; 
} Nodo;


typedef Nodo * Pila;

void push(int, Pila *);

void print(Pila);

void printReverse(Pila);

int size(Pila);

int peek(Pila);

int pop(Pila* );

// Pila push (int, Pila);

int main(){

	Pila p = NULL;
	int x;
	do{
		scanf("%d",&x);
		push(x,&p);
		//p=push(x,push(x,p))
	} while(x>=0);

	printf("la pila contiene %d elementi:\n", size(p));
	print(p);

	printf("L'elemento successivo nella pila è: %d \n",peek(p));
	x=pop(&p);
	printf("Ho rimosso l'elemento %d \n", x);
	printf("L'elemento successivo nella pila è: %d \n",peek(p));
	
	printf("la pila contiene %d elementi:\n", size(p));
	print(p);
	
} 

int pop (Pila * p){
	if(*p ==NULL)
		return 0;

	int val=(*p)->val;

	Nodo* temp=*p;
	*p=(*p)->next;
	free(temp);

	return val;

}

int peek(Pila p){
	if(p==NULL){
		return 0;
	}
	return p->val;
}

void push(int n, Pila* p){
	
	Nodo* nd = (Nodo*)malloc(sizeof(Nodo));
	if(nd==NULL){
		printf("Errore");
		exit(1);
	}

	nd->val=n;
	nd->next=*p;
	
	*p=nd;
}

void print(Pila p){
	if(p==NULL)
		return;
	printf("%d\n",p->val);
	print(p->next);

}


void printReverse(Pila p){
	if(p==NULL)
		return;

	printReverse(p->next);
	printf("%d\n",p->val);

}


int size(Pila p){
	int s=0;

	while(p!=NULL){
		s++;
		p=p->next;
	}

	return s;
}






