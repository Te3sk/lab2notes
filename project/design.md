# TODO
- [x] Aggiungere `name_bib`, `file_record` e `W` all'invocazione del server
    - [x] tutti e 3 i parametri da linea di comando sono obbligatori
    - [x] controllare che il file (`file_record`) esista
- [x] SERVER: il server deve aggiornare i file di `file_record` sovrascrivendo con i record aggiornati della struttura condivisa
- [x] SERVER: i libri per cui la scadenza del prestito è passata sono considerati disponibili e quindi il campo `prestito` non viene scritto nel file
- [x] Il server registra un file di log (`./name_bib.log`) in cui per ogni richiesta effettuata da un client si registrano il numero dei record inviati, dei prestiti effettuati e le info relative a ciascun record
    - [x] (**DA FARE TESTING PIÙ APPROFONDITO**)LOG: ogni richiesta processata da ogni server viene registrata all'interno di un file di log (`name_bib.log`).
    - [x] All'inizio dell'eleaborazione il server tronca il file (elimina eventuali contenuti precedenti), poi per ogni richiesta registra delle linee
    - [x] (**DA FARE TESTING PIÙ APPROFONDITO**)richieste di tipo MSG_QUERY:
        scrive `QUERY N` dove `N` è il numero totale di record inviati, preceduti dalle stringhe contenenti i record inviati. In caso non venga inviato nessun record si scrive solo `QUERY 0`
    - [x] (**DA FARE TESTING PIÙ APPROFONDITO**)richieste di tipo MSG_LOAN:
        analogo a MSG_QUERY: `stringa_con_record\nLOAN N` o `LOAN 0`
- [x] mutex per scrivere su BibData (gestire concorrenza per la struttura dati condivisa contenente i records)
- [x] terminazione del server
    - [x] il server termina quando riceve un segnale SIGINT o SIGTERM
    - [x] si attende la terminazione dei thread worker
    - [x] si termina la scrittura del file di log
    - [x] si registra il nuovo file record
    - [x] si elimina la socket del server
- [ ] la lista di tutti i server disponibili è inclusa in un file di configurazione (`bib.conf`) che include tutti i nomi dei vari server attivi, e i dati necessari per la connessione ai rispettivi socket
    - [ ] Il client legge il file `bib.conf`. Poi interroga tutte le biblioteche connettendosi sulla socket e mandando una richeista
- [ ] PROTOCOLLO DI COMUNICAZIONE: capire, in caso di MSG_ERROR o MSG_NO, cosa fa/stampa il client
- [ ] SCRIPT DI CONTROLLO: `bibaccess` è uno script bash che elabora off-line i file di log generati dai bibserver
    - [ ] può essere invocato con due opzioni: 
        - `$ ./bibaccess --quey log1...logN`
        - `$ ./bibaccess --loan log1...logN`
    - [ ] dove `log1...logN` sono file di testo (ASCII) che contengono i log dei record prestati dai vari server
    - [ ] con le opzioni con doppio trattino (`--query` e `--loan`) si può richiedere il numero complessivo di richieste e di prestiti nei file specificati
    - [ ] lo script deve:
        - [ ] controllare la validità dei suoi argomenti
        - [ ] scorrere i file di log
        - [ ] individuare le info richieste
        - [ ] stampa una riga per file di log ricevuto in input
        - [ ] alla fine stampa le parole QUERY e LOAN e il loro valore complessivo (esempio sotto)
```
$ ./bibaccess --query bib1.log bib2.log bib3.log

bib1.log 5

bib2.log 7

bib3.log 0

QUERY 12
```
- [ ] MAKEFILE
    - [x] makefile di prova
    - [ ] deve avere, fra gli altri, i target:
        - [x] `all` (generare tutti gli eseguibili)
        - [x] `clean` (eliminare i file eseguibili)
        - [ ] `test` (ciclo completo di test)
            - [ ] lancia 5 server con database diversi e con un numero variabile di thread worker
            - [ ] wait 1 sec
            - [ ] lancia 40 client con richieste di tipo query e loan
                - [ ] i client vanno lanciati 5 alla volta con con un attesa di 1 sec tra loro
            - [ ] dopo l'ultimo gruppo lancia un SIGINT al server
            - [ ] wait 10 sec
            - [ ] lancia lo script `bibaccess` sui 5 file di log



# Server
1. **Socket** creation
2. **Shared queue** building
3. **log file** writing
4. **input format check**
5. **thread worker** creation
6. shared **data structure** building
7. worker function
    1. take **request** from queue
    2. **loan time** check
    3. **record file** update
    4. **book searcher**
    5. send **response**
        1. send **type**
        2. send **lenght**
        3. send **data**
8. **terminator**
    1. waiting for all **workers to finish**
    2. waiting for **log file updating**
    3. recording new **record file**
    4. **close socket**

# Client
1. **Socket** creation
2. check **input format**
3. **parse request**
4. **send request**
    1. send **type**
    2. send **lenght**
    3. send **data**

# Formato request
- numero di parametri
- parametri come stringa del tipo "campo:valore;campo:valore;...;p" (campo p presente se si chiede il prestito)
- bool (o simile) per indicare se è richiesto il prestito o no

# Nomi campo possibili
1. autore - a
2. titolo - t
3. editore - e
4. anno - y
5. nota - n
6. collocazione - c
7. luogo pubblicazione - p
8. descrizione fisica - d