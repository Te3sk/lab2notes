CC = gcc
CFLAGS = -Wall -Wextra
SERVER_SRC = server.c
CLIENT_SRC = client.c
SERVER_BIN = server.out
CLIENT_BIN = client.out
LIB_SRC = lib/bib_ds.c lib/pars.c lib/queue.c

all: server client

server: $(SERVER_SRC) $(LIB_SRC)
	$(CC) $(SERVER_SRC) $(LIB_SRC) $(CFLAGS) -o $(SERVER_BIN) 

client: $(CLIENT_SRC) $(LIB_SRC)
	$(CC) $(CLIENT_SRC) $(LIB_SRC) $(CFLAGS) -o $(CLIENT_BIN)

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)

clean_sock:
	rm socket/temp_sock