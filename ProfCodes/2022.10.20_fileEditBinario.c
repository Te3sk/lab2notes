#include <stdio.h>
#define N 10
int main(){

	FILE* file=fopen("f.dat","r+b");
	int x;

	if(file){
		fseek(file, 2*sizeof(int),SEEK_SET);
		scanf("%d",&x);
		fwrite(&x,sizeof(int),1,file);

		fseek(file, 0,SEEK_SET);
		while(!feof(file)){
			fread(&x,sizeof(int),1,file);
			printf("%d ",x);
		}
		
		fclose(file);

	}else {
		perror("Apertura file f.dat: ");
	}
}