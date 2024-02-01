// Implementare una chat dove il server inoltra i messaggi
// dei clienti a tutti gli altri clienti connessi in quel
// momento. I clienti si connettono e mandano messaggi
// multipli presi dall'utente da tastiera

// Variante 1: implementare il server single thread

// TODO - problemi nell'uso della socket condivisa
// ! il client invia il messaggio ma il server non lo stampa e non lo reinvia, 
// ! quando viene chiuso il server il client riceve il messaggio all'infinito, ma solo il client che l'ha inviato

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/select.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define BUFFER_SIZE 100
#define MAX_CLIENTS 10
#define PORT 2222

int main()
{
    // Creare un socket per il server
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int client_FDs[MAX_CLIENTS], n_cli = 0;

    // Inizializzare i file descriptor
    fd_set readfds, allFDs;
    FD_ZERO(&allFDs);
    FD_SET(server_fd, &allFDs);
    int max_fd = server_fd;

    int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
    if (activity < 0) {
        perror("select:");
        exit(EXIT_FAILURE);
    }

    char buf[BUFFER_SIZE];

    // Inizializzare l'indirizzo del server
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("192.168.111.47");

    // Bindare il socket al server
    printf("binding...\n");
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Apro il server
    printf("listening...\n");
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Inizializzare la select
    while (1)
    {
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd < 0) {
            perror("accept:");
            exit(EXIT_FAILURE);
        }
        client_FDs[n_cli] = client_fd;
        n_cli++;
        FD_SET(client_fd, &readfds);
        printf("Nuovo cliente connesso (%d tot)\n", n_cli);

        if (client_fd > max_fd) {
            max_fd = client_fd;
        }

        while(1) {
            memset(buf, 0, sizeof(buf));
            int bytesread = read(client_fd, buf, sizeof(buf));
            if (bytesread <= 0) {
                printf("il cliente si è disconnesso\n");
                FD_CLR(client_fd, &readfds);
                break;
            }

            for (int i = 0; i < n_cli; i++) {
                if (FD_ISSET(client_fd, &readfds) && client_FDs[i] != client_fd) {
                    write(client_FDs[i], buf, sizeof(buf));
                }
            }
        }
    }




        // ----------------------------------------------------------------------------------------------------------------
        
        // // Inizializzare i file descriptor
        // readfds = allFDs;

        // // Eseguire la select
        // if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0) {
        //     perror("select");
        //     exit(EXIT_FAILURE);
        // }

        // // Per ogni file descriptor
        // for (int i = 0; i < max_fd; i++)
        // {
        //     // Se il file descriptor e' stato modificato
        //     if (FD_ISSET(i, &readfds))
        //     {
        //         // Se il file descriptor e' quello del server
        //         if (i == server_fd)
        //         {
        //             printf("\t\tcaso server\n");
        //             // Apro una connessione
        //             int client_fd = accept(server_fd, NULL, NULL);
        //             printf("Un cliente prova a connettersi...\n");
        //             // Se l'operazione e' fallita
        //             if (client_fd < 0)
        //             {
        //                 perror("accept:");
        //                 continue;
        //             }

        //             printf("Connessione riuscita!\n");

        //             // Stampare il numero del nuovo client
        //             // printf("Nuovo client connesso: %d\n", client_fd);

        //             // Aggiungere il nuovo file descriptor
        //             FD_SET(client_fd, &allFDs);
        //             // Se il nuovo file descriptor e' maggiore del vecchio
        //             if (client_fd > max_fd)
        //                 max_fd = client_fd;
        //         }
        //         else
        //         {
        //             printf("\t\tcaso client\n");
        //             char buf[BUFFER_SIZE];
        //             int bytesread = read(i, buf, BUFFER_SIZE);
        //             printf("Read %d bytes", bytesread);
        //             if (bytesread <= 0)
        //             {
        //                 if (bytesread == 0)
        //                 {
        //                     printf("Il cliente ha chiuso la connessione\n");
        //                 }
        //                 else if (bytesread == -1)
        //                 {
        //                     perror("read:");
        //                 }
        //                 close(i);
        //                 FD_CLR(i, &allFDs);
        //                 if (i == max_fd)
        //                 {
        //                     do
        //                     {
        //                         max_fd--;
        //                     } while (!FD_ISSET(max_fd, &allFDs));
        //                 }
        //             } else {
        //                 printf("Il cliente ha inviato %s\n", buf);
        //                 write(i, buf, bytesread); // funzione echo, vediamo se funziona poi cambiamo
        //             }

        //             close (i);
        //             FD_CLR(i, &allFDs);
        //             if (i == max_fd) {
        //                 do{max_fd--;}while(!FD_ISSET(max_fd, &allFDs));
        //             }
        //         }
        //     }
        // }
    
    close(server_fd);

    return 0;
}