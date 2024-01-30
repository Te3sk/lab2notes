// Variazione 2: implementare un server multithread dove un
// thread worker nuovo viene creato per ogni cliente

#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>

#define N 100

void* worker(void* arg) {
    char buf[N];    

    int fd = (int)arg;
    read(fd, buf, N);
    printf("Il cliente ha inviato: %s\n", buf);
    write(fd, buf, strlen(buf)+1);
    close(fd);
    return NULL;
}

int main() {
    //Creazione del socket
    int server=socket(AF_INET, SOCK_STREAM, 0);

    //Creazione del server
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2222);
    server_addr.sin_addr.s_addr = inet_addr("192.168.111.47");

    //Binding del socket
    bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server, 10);

    //Creazione della struttura di descrizione delle fd
    fd_set allFDs, readFDs;
    FD_ZERO(&allFDs);
    FD_SET(server, &allFDs);
    int fdMax = server;

    //Aspetto per la connessione
    while(1) {
        int clientFD = accept(server, NULL, NULL);
        if (clientFD ==- 1) {
            perror("Errore nella accept");
            continue;
        }
        // printf("E' arrivato un cliente\n");
        //Creazione del thread worker
        pthread_t tid;
        pthread_create(&tid, NULL, worker, (void*)clientFD);
        pthread_detach(tid);
    }

    //Chiusura del socket
    close(server);

    return 0;
}