// Variazione 1: implementare il server in modo single thread

#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>

#define N 100
#define IP_ADDRESS "192.168.111.47"
#define PORT 2222

int main(){

	//Creazione del socket
	int server=socket(AF_INET,SOCK_STREAM,0);
	
	//Creazione della struttura dati per la descrizione del server
	struct sockaddr_in serverAddr;
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons(PORT);
	serverAddr.sin_addr.s_addr=inet_addr(IP_ADDRESS);
	
	//Bind del socket
	bind(server,(struct sockaddr*)&serverAddr,sizeof(serverAddr));

	//Listen del socket
	listen(server, 10);
	
	//Creazione dei file descriptor set
	fd_set allFDs, readFDs;
	FD_ZERO(&allFDs);
	FD_SET(server,&allFDs);
	int fdMax=server;
	
	//Ciclo principale del programma
	while(1){
		//Copia del file descriptor set
		readFDs=allFDs;
		//Attesa di un evento sull'intero file descriptor set
		select(fdMax+1,&readFDs,NULL,NULL,NULL);

		//Ciclo sui file descriptor
		for (int i=0;i<fdMax+1;i++){
			//Controllo se il file descriptor e' attivo
			if (FD_ISSET(i,&readFDs)){
				//Caso server
				if (i==server){

					//Aspetto nuovo client
					int clientFD=accept(server, NULL,NULL);
                    if (clientFD == -1) {
                        perror("Errore nella accept");
                        continue;
                    }
					printf("E' arrivato un cliente \n");
					FD_SET(clientFD,&allFDs);
					if(clientFD>fdMax)
						fdMax=clientFD;

				//Caso client
				} else {
                    char buf[N];
                    int bytesread = read(i,buf,N);
                    printf("read %d bytes\n", bytesread);
                    if (bytesread <= 0) {
                        if (bytesread == 0){
                            printf("Il cliente ha chiuso la connessione\n");
                        } else if (bytesread == -1) {
                            perror("Errore nella read dal cliente");
                        }
                        close(i);
                        FD_CLR(i,&allFDs);
                        if (i == fdMax) {
                            do{
                                fdMax--;
                            }while(!FD_ISSET(fdMax,&allFDs));
                        }
                    } else {
    					printf("Il cliente ha inviato %s\n",buf);
	    				write(i,buf,strlen(buf)+1);
                    }
					close(i);
					FD_CLR(i,&allFDs);
					if(i ==fdMax)
						do{fdMax--;}
						while(!FD_ISSET(fdMax,&allFDs));

				}
				
			}
		}
		

	}
	close(server);
	
	return 0;

}