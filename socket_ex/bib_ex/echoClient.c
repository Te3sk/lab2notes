#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>


#define N 100
#define IP_ADDRESS "192.168.111.47"

int main(){
	//Creo un socket
	int server=socket(AF_INET,SOCK_STREAM,0);
	
	//Creo un indirizzo per il server
	struct sockaddr_in serverAddr;
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons(2222);
	serverAddr.sin_addr.s_addr=inet_addr(IP_ADDRESS);

	//Tento di connettersi al server
	int conn=0;
	while((conn=connect(server, (struct sockaddr*)&serverAddr,sizeof(serverAddr)))==-1 && errno==ENOENT) {sleep(1);}

	//Se non riesco a connettersi allora stampo un errore
	if (conn==-1){
		printf("errore di connesione: %s", strerror(errno));
		exit(1);
	} else {
        printf("connessione riuscita\n");
    }

	//Leggo un input
	char buff[N];
	scanf("%s",buff);

	//Invio il messaggio
	write(server,buff,(strlen(buff)+1)*sizeof(char));
	//Ricevo la risposta
	read(server,buff, N);
	printf("Server ha risposto: %s\n",buff);
	close(server);
	
	return 0;

}