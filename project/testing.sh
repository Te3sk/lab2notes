#!/bin/bash

# Salvataggio del primo e secondo argomento in variabili dedicate
server_path="$1"
client_path="$2"
bibaccess_path="$3"

# Verifica che sia stati forniti entrambi i percorsi
if [ -z "$server_path" ] || [ -z "$client_path" ] || [ -z "$bibaccess_path" ]; then
    echo "Errore: devi fornire sia il percorso del server, sia quello del client che quello di bibaccess."
    echo "Uso: $0 <percorso_server> <percorso_client> <percorso bibaccess>"
    exit 1
fi


# Avvio dei server 
$server_path bib_1 bibData/bib1.txt 1 &
$server_path bib_2 bibData/bib2.txt 2  &
$server_path bib_3 bibData/bib3.txt 3 &
$server_path bib_4 bibData/bib4.txt 4 &
$server_path bib_5 bibData/bib5.txt 5  &

sleep 1

# Ciclo per l'avvio dei client 8 volte con un'attesa di 1 secondo tra un ciclo e l'altro.
for i in {1..8}; do
    $client_path --luogo_pubblicazione="New York" &
    $client_path --anno="1998"  &
    $client_path --autore="Pagli, Linda" -p  &
    $client_path --autore="Di Ciccio, Antonio" --titolo="Manuale di architettura pisana" --editore="Palestro" --anno="1990" --volume="1345" --scaffale="A.west.2" -p  &
    $client_path --autore="Bentley, Jon"  &
    sleep 1
done

# Chiusura dei server
pkill -u $(whoami) -INT $(basename $server_path)

sleep 10

#lancio bibaccess
# $bibaccess_path --query bib_1.log bib_2.log bib_3.log bib_4.log bib_5.log
# $bibaccess_path --loan bib_1.log bib_2.log bib_3.log bib_4.log bib_5.log