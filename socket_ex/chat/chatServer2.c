// Implementare una chat dove il server inoltra i messaggi
// dei clienti a tutti gli altri clienti connessi in quel
// momento. I clienti si connettono e mandano messaggi
// multipli presi dall'utente da tastiera

// Variante 2: implementare il server multi thread dove
// all'arrivo di ogni cliente viene creato un thread
// worker nuovo che gestisce solo quel cliente

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/select.h>

#define BUFFER_SIZE 100

void *worker(void *arg);



int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2222);
    server_addr.sin_addr.s_addr = inet_addr("192.168.111.47");

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 10);

    fd_set readfds;
    FD_ZERO(&readfds);
    

    return 0;
}

void *worker(void *arg)
{
    char buf[BUFFER_SIZE];

    int client_fd = (int)arg;
    read(client_fd, buf, BUFFER_SIZE);
    printf("il cliente ha inviato: %s\n", buf);
    write(client_fd, buf, strlen(buf) + 1);
    close(client_fd);

    return NULL;
}