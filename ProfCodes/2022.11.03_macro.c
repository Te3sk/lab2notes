#include <stdio.h>

#define MAX(A,B) if (A>B) printf("%d\n",A); else printf("%d\n",B);


int main(){

	int x,y;
	scanf("%d %d",&x,&y);

	MAX(x,y)

}

  