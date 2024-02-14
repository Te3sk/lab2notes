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





   