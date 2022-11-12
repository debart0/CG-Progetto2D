DESCRIZIONE:
Il progetto consiste in un gioco dove è possibile controllare una navicella
(nel codice la chiamo anche pod di atterraggio) che deve cercare di attraversare
la schermata senza collidere con gli asteroidi che fluttuano nello spazio.
In alto a sinistra, abbiamo un contatore di VITE (ogni volta che ci si scontrerà
con un asteroide se ne perderà una) e di PUNTI, guadagnati ogni volta che si
riuscirà ad attraversare la schermata.

COMANDI:
A - D: movimento orizzontale della navicella (si possono usare anche le frecce direzionali)
P: pausa
ESC: chiusura del gioco
MOUSE: a seconda della posizione del mouse sulla finestra è possibile
muovere le stelle sullo sfondo e influenzarne il colore. Più precisamente, andare
a destra aumenterà la quantità di Rosso e andare in basso quella di Blu.

ISTRUZIONI PER LA COMPILAZIONE:  
Come nelle lezioni di laboratorio, è necessario copiare i file nella cartella
del progetto e aggiungere i .cpp ai file di origine e i .h ai file di intestazione.
È necessario installare nupengl e glm dalla console nuget.
Dato che il progetto include scritte con la libreria freetype, è necessario aggiungerla
nell'input del linker e compilare a 64 bit (come nelle lezioni).
