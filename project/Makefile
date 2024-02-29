CC = gcc
CFLAGS = -Wall -Wextra
SERVER_SRC = server.c
CLIENT_SRC = client.c
SERVER_BIN = server.out
CLIENT_BIN = client.out
BIBACCESS_BIN = bin/bibaccess
LIB_SRC = lib/bib_ds.c lib/pars.c lib/queue.c lib/log_func.c

TEST_CLIENT_PARAM = --author="Luccio: Fabrizio" --title="Mathematical and Algorithmic Foundations of the Internet" -p

RANDOM := $(shell awk 'BEGIN{srand(); print int(1 + rand() * 5)}')
TEST_SERVER_PARAM = bib_$$i bib$$i.txt $(RANDOM)


all: server client

server: $(SERVER_SRC) $(LIB_SRC)
	$(CC) $(SERVER_SRC) $(LIB_SRC) $(CFLAGS) -g -o $(SERVER_BIN) 

server_run:
	./$(SERVER_BIN) bib_1 bibData/bib1.txt 4

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
	./$(SERVER_BIN) bib_1 bibData/bib1.txt $(RANDOM) & \
	./$(SERVER_BIN) bib_2 bibData/bib2.txt $(RANDOM) & \
	./$(SERVER_BIN) bib_3 bibData/bib3.txt $(RANDOM) & \
	./$(SERVER_BIN) bib_4 bibData/bib4.txt $(RANDOM) & \
	./$(SERVER_BIN) bib_5 bibData/bib5.txt $(RANDOM) & \
	sleep 1 && \
	for i in {1..5}; do \
		(./(CLIENT_BIN) --autore="Stevens, W. Richard" --title=" Unix Network Programming (Second Edition): Volume1 Networking APIs: Sockets and XTI" -p &\
		./(CLIENT_BIN) --autore="Fruttero, Carlo" luogo_pubblicazione="Milano" &\
		./(CLIENT_BIN) --title="Dias De Combate" --editore="Planeta Mexicana" &\
		./(CLIENT_BIN) --title="La soledad del manager" --anno="1977" -p &\
		./(CLIENT_BIN) --autore="Kernighan, Brian W." --autore="Baudo, Pippo" -p &\
		./(CLIENT_BIN) --autore="Baudo, Pippo" --title="L'ultimo dei baudi" &\
		./(CLIENT_BIN) &\
		./(CLIENT_BIN) --parametrononvalido &\
		./(CLIENT_BIN) --author="Dick, Philip K." &\
		sleep 1);\
	done && \
	killall -INT bibserver &&\
	sleep 10 &&\
	./$(BIBACCESS_BIN) --query bib_1.log bib_2.log bib_3.log bib_4.log bib_5.log 
	./$(BIBACCESS_BIN) --loan bib_1.log bib_2.log bib_3.log bib_4.log bib_5.log