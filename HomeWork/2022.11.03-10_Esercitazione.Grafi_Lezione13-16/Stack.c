#include "Stack.h"
#include <stdlib.h>
#include <stdio.h>

void push(Stack* s, int n){
	StackNode* node=(StackNode*)malloc(sizeof(StackNode));
	if(node==NULL){
		printf("Memory error\n");
		exit(1);
	}

	node->index=n;
	node->next=*s;
	*s=node;

}

int pop(Stack* s){
	if(*s==NULL)
		return -1;
	int res=(*s)->index;
	Stack temp=*s;
	*s=(*s)->next;
	free(temp);
	return res;
}


