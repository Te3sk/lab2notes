CC = gcc
CFLAGS = -Wall -Wextra
SERVER_SRC = server.c
CLIENT_SRC = client.c
SERVER_BIN = server.out
CLIENT_BIN = client.out
LIB_SRC = lib/bib_ds.c lib/pars.c lib/queue.c lib/log_func.c

TEST_SERVER_PARAM = temp_bib bibData/bib1.txt 4
TEST_CLIENT_PARAM = --author="Luccio, Fabrizio" --title="Mathematical and Algorithmic Foundations of the Internet" -p

all: server client

server: $(SERVER_SRC) $(LIB_SRC)
	$(CC) $(SERVER_SRC) $(LIB_SRC) $(CFLAGS) -o $(SERVER_BIN) 

server_run:
	./$(SERVER_BIN) $(TEST_SERVER_PARAM)

client: $(CLIENT_SRC) $(LIB_SRC)
	$(CC) $(CLIENT_SRC) $(LIB_SRC) $(CFLAGS) -o $(CLIENT_BIN)

client_run:
	./$(CLIENT_BIN) $(TEST_CLIENT_PARAM)

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)

clean_sock:
	rm socket/*