CC = gcc
CFLAGS = -Wall -Wextra
SERVER_SRC = server.c
CLIENT_SRC = client.c
SERVER_BIN = server.out
CLIENT_BIN = client.out
BIBACCESS_BIN = bin/bibaccess
LIB_SRC = lib/bib_ds.c lib/pars.c lib/queue.c lib/log_func.c

# TEST_CLIENT_PARAM = --author="Luccio, Fabrizio" --title="Mathematical and Algorithmic Foundations of the Internet"
# TEST_CLIENT_PARAM = --autore="Kernighan, Brian W." --titolo="Il linguaggio C (seconda Edizione)" -p
TEST_CLIENT_PARAM = --autore="Aho, Alfred V." --titolo="Compilers: Principles, Techniques, and Tools" -p


RANDOM := $(shell awk 'BEGIN{srand(); print int(1 + rand() * 5)}')
TEST_SERVER_PARAM = bib_$$i bib$$i.txt $(RANDOM)


all: server client

server: $(SERVER_SRC) $(LIB_SRC)
	$(CC) $(SERVER_SRC) $(LIB_SRC) $(CFLAGS) -g -o $(SERVER_BIN) 

server_run:
	./$(SERVER_BIN) bib_2 bibData/bib4.txt 1

client: $(CLIENT_SRC) $(LIB_SRC)
	$(CC) $(CLIENT_SRC) $(LIB_SRC) $(CFLAGS) -o $(CLIENT_BIN)

client_run:
	./$(CLIENT_BIN) $(TEST_CLIENT_PARAM)

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)

clean_sock:
	rm socket/*

client_test:
	./(CLIENT_BIN) --autore="Stevens, W. Richard" --title=" Unix Network Programming (Second Edition): Volume1 Networking APIs: Sockets and XTI" -p &\
	./(CLIENT_BIN) --autore="Fruttero, Carlo" luogo_pubblicazione="Milano" &\
	./(CLIENT_BIN) --title="Dias De Combate" --editore="Planeta Mexicana" &\
	./(CLIENT_BIN) --title="La soledad del manager" --anno="1977" -p &\
	./(CLIENT_BIN) --autore="Kernighan, Brian W." --autore="Baudo, Pippo" -p &\
	./(CLIENT_BIN) --autore="Baudo, Pippo" --title="L'ultimo dei baudi" &\
	./(CLIENT_BIN) &\
	./(CLIENT_BIN) --parametrononvalido &\
	./(CLIENT_BIN) --author="Dick, Philip K." &\
	sleep 1
	
server_test:
	./$(SERVER_BIN) bib_1 bibData/bib1.txt $(RANDOM) & \
	./$(SERVER_BIN) bib_2 bibData/bib2.txt $(RANDOM) & \
	./$(SERVER_BIN) bib_3 bibData/bib3.txt $(RANDOM) & \
	./$(SERVER_BIN) bib_4 bibData/bib4.txt $(RANDOM) & \
	./$(SERVER_BIN) bib_5 bibData/bib5.txt $(RANDOM) & \

# TODO - capire come aprire le istanze di client e di server 
test: all
	./testing.sh ./$(SERVER_BIN) ./$(CLIENT_BIN) ./bibaccess.sh


testing_logs: all
	./test_logs.sh ./$(SERVER_BIN) ./$(CLIENT_BIN) ./bibaccess.sh