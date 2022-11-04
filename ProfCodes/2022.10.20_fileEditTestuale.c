///file incompleto da finire a casa

#include <stdio.h>
#define N 10
int main(){

	FILE* file=fopen("f.txt","r+");
	int x;

	if(file){
		for(int i=0;i<2;i++)
			fscanf(file,"%*d,");

		printf("%ld\n",ftell(file));
		scanf("%d",&x);
		fprintf(file,"%d,",x);
/*
		fseek(file, 0,SEEK_SET);
		while(!feof(file)){
			fscanf(file,"%d,"&x);
			printf("%d ",x);
		}*/
		
		fclose(file);

	}else {
		perror("Apertura file f.dat: ");
	}
}