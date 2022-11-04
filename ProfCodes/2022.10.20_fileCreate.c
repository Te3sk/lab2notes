#include <stdio.h>
#define N 10
int main(){

	FILE* file=fopen("f.dat","wb");

	if(file){
		int x;
		for(int i=0;i<N;i++){
			scanf("%d",&x);
			fwrite(&x,sizeof(int),1,file);
		}
		fclose(file);

	}else {
		perror("Apertura file f.txt: ");
	}
}