//ordinamento stringhe
#define N 100

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int comp(const void * e1, const void * e2){
	
	char** s1=(char**)e1;
	char** s2=(char**)e2;

	return strcmp(*s1,*s2);
}

int main(){

	int n;

	scanf("%d",&n);

	char* stringhe[n];

	for(int i=0;i<n;i++){	
		stringhe[i]=(char*)malloc(N*sizeof(char));
		if(stringhe[i]==NULL)
				exit(1);
		scanf("%s",stringhe[i]);
	}	

	qsort(stringhe,n,sizeof(char*),comp);
	
	for(int i=0;i<n;i++){	
		printf("%s\n",stringhe[i]);
	}

	for(int i=0;i<n;i++){
		free(stringhe[i]);
	}

}
