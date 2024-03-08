CC = gcc
CFLAGS = -Wall -Wextra
SERVER_SRC = bibserver.c
CLIENT_SRC = bibclient.c
SERVER_BIN = bibserver.out
CLIENT_BIN = bibclient.out
BIBACCESS_BIN = bin/bibaccess
LIB_SRC = lib/bib_ds.c lib/pars.c lib/queue.c lib/log_func.c

TEST_CLIENT_PARAM = --autore="Aho, Alfred V." --titolo="Compilers: Principles, Techniques, and Tools" -p


RANDOM := $(shell awk 'BEGIN{srand(); print int(1 + rand() * 5)}')
TEST_SERVER_PARAM = bib_$$i bib$$i.txt $(RANDOM)


all: server client

server: $(SERVER_SRC) $(LIB_SRC)
	$(CC) $(SERVER_SRC) $(LIB_SRC) $(CFLAGS) -g -o $(SERVER_BIN) -lpthread

server_run:
	./$(SERVER_BIN) bib_2 bibData/bib4.txt 1

client: $(CLIENT_SRC) $(LIB_SRC)
	$(CC) $(CLIENT_SRC) $(LIB_SRC) $(CFLAGS) -o $(CLIENT_BIN) -lpthread

client_run:
	./$(CLIENT_BIN) $(TEST_CLIENT_PARAM)

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)

clean_sock:
	rm socket/*

test: all
	./testing.sh ./$(SERVER_BIN) ./$(CLIENT_BIN) ./bibaccess.sh
