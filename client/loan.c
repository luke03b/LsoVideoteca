# include "loan.h"

void add_to_cart(int sock, int* num_film, int* carrello){
    int id_film = 0;
    printf("Inserisci l'ID del film da aggiungere al carrello: ");

    if (scanf("%d", &id_film) != 1) {
        // Input non valido (non è un numero)
        while (getchar() != '\n'); // Pulisci il buffer di input
        id_film = 0; // Imposta una scelta non valida per entrare nel default
    } else {
        getchar(); // Consuma il carattere newline solo se l'input era valido
    }

    // Controlla se l'ID del film è valido
    if (id_film <= 0) {
        printf("ID film non valido. Riprova.\n");
        printf("\nPremi invio per continuare...");
        getchar();
        return;
    }

    // Controlla se il film è già presente nel carrello
    if (is_film_already_in_cart(id_film, num_film, carrello)) {
        printf("Il film con ID %d è già presente nel carrello.\n", id_film);
        printf("\nPremi invio per continuare...");
        getchar();
        return;
    }

    // Controlla se il carrello è pieno
    if (*num_film < 5) {
        carrello[*num_film] = id_film;
        (*num_film)++;
        printf("Film con ID %d aggiunto al carrello.\n", id_film);
    } else {
        printf("Carrello pieno. Non puoi aggiungere più di 5 film.\n");
    }

    printf("\nPremi invio per continuare...");
    getchar();
}

// Funzione per visualizzare film in base a un array di ID
void show_cart(int sock, int* num_film, int* id_utente_loggato, int* carrello) {
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 20] = {0};
    int bytes_received;

    while (1) {
        clear_screen();
        printf("===== CARRELLO =====\n\n");
        
        if (*num_film <= 0) {
            printf("Nessun film nel carrello.\n");
            printf("\nPremi invio per tornare al menu principale...");
            getchar();
            return;
        }
        
        // Costruiamo una stringa con tutti gli ID separati da virgole
        char id_list[200] = {0};
        for (int i = 0; i < *num_film; i++) {
            char id_str[10];
            snprintf(id_str, sizeof(id_str), "%d", carrello[i]);
            
            // Aggiungi virgola se non è il primo ID
            if (i > 0 && i < *num_film) {
                strcat(id_list, ",");
            }
            strcat(id_list, id_str);
        }
        
        // Crea la richiesta di ricerca (3 = ricerca per ID multipli)
        snprintf(request, BUFFER_SIZE, "SEARCH:4:%s", id_list);
        send(sock, request, strlen(request), 0);
        
        bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received <= 0) {
            printf("Errore nella ricezione dei dati dal server.\n");
            printf("\nPremi invio per continuare...");
            getchar();
            return;
        }
        
        buffer[bytes_received] = '\0';
        
        if (strncmp(buffer, "SEARCH_FAIL", 11) == 0) {
            printf("Nessun film trovato con gli ID specificati.\n");
        } else {
            printf("Film trovati nel carrello:\n\n");
            display_films(buffer, 1);
        }


        int choice = 0;
        // Mostra le opzioni del carrello
        printf("\n\n===== OPZIONI =====\n");
        printf("1. Effettua il Check-out\n");
        printf("2. Svuota Carrello\n");
        printf("3. Torna al menu principale\n");
        printf("Scelta: ");
        if (scanf("%d", &choice) != 1) {
            // Input non valido (non è un numero)
            while (getchar() != '\n'); // Pulisci il buffer di input
            choice = 0; // Imposta una scelta non valida per entrare nel default
        } else {
            getchar(); // Consuma il carattere newline solo se l'input era valido
        }

        switch (choice) {
            case 1:
                check_out(sock, num_film, id_utente_loggato, carrello);
                printf("\nPremi invio per continuare...");
                getchar();
                break;
            case 2:
                *num_film = 0;
                printf("\nCarrello svuotato.");
                printf("\nPremi invio per continuare...");
                getchar();
                break;
            case 3:
                return;
            default:
                printf("Scelta non valida. Riprova.\n");
                printf("\nPremi invio per continuare...");
                getchar();
        
        }
    }
}

void check_out(int sock, int* num_film, int* id_utente_loggato, int* carrello){
    for (int i = 0; i < *num_film; i++)
    {
        char request[BUFFER_SIZE];
        char buffer[BUFFER_SIZE] = {0};
        // Crea la richiesta di login
        snprintf(request, BUFFER_SIZE, "NOLEGGIO:%d:%d", *id_utente_loggato, carrello[i]);
        
        // Invia la richiesta al server
        send(sock, request, strlen(request), 0);
        
        // Ricevi la risposta
        read(sock, buffer, BUFFER_SIZE);
        
        // Controlla la risposta del server
        if (strcmp(buffer, "LOAN_OK") == 0) {
            printf("Hai noleggiato il film con ID %d\n", carrello[i]);
        } else if (strcmp(buffer, "LOAN_FAIL") == 0) {
            printf("Errore nel noleggio del film\n");
        } else {
            printf("Errore nella comunicazione con il server: %s\n", buffer);
        }
    }
    *num_film = 0; // Svuota il carrello dopo il check-out
    printf("Carrello svuotato dopo il check-out.\n");
}


int is_film_already_in_cart(int id_film, int* num_film, int* carrello) {
    for (int i = 0; i < *num_film; i++) {
        if (carrello[i] == id_film) {
            return 1; // Film già presente nel carrello
        }
    }
    return 0; // Film non presente nel carrello
}

void retrieve_loaned_films(int sock, int* id_utente_loggato) {

    while (1)
    {
        clear_screen();
        printf("===== FILM NOLEGGIATI =====\n\n");
        char request[BUFFER_SIZE];
        char buffer[BUFFER_SIZE * 20] = {0};
        int bytes_received;
        
        // Crea la richiesta di ricerca (5 = ricerca film noleggiati utente loggato)
        snprintf(request, BUFFER_SIZE, "SEARCH:5:%d", *id_utente_loggato);
        send(sock, request, strlen(request), 0);
        
        bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received <= 0) {
            printf("Errore nella ricezione dei dati dal server.\n");
            printf("\nPremi invio per continuare...");
            getchar();
            return;
        }
        
        buffer[bytes_received] = '\0';
        
        if (strncmp(buffer, "SEARCH_FAIL", 11) == 0) {
            printf("Nessun film noleggiato\n");
            printf("\nPremi invio per tornare al menu principale...");
            getchar();
            return;
        } else {
            display_films(buffer, 0);
        }
    
        int choice = 0;
        // Mostra le opzioni del carrello
        printf("\n\n===== OPZIONI =====\n");
        printf("1. Restituisci film\n");
        printf("2. Torna al menu principale\n");
        printf("Scelta: ");

        if (scanf("%d", &choice) != 1) {
            // Input non valido (non è un numero)
            while (getchar() != '\n'); // Pulisci il buffer di input
            choice = 0; // Imposta una scelta non valida per entrare nel default
        } else {
            getchar(); // Consuma il carattere newline solo se l'input era valido
        }


        switch (choice) {
            case 1:
                // Funzione per restituire un film
                restituisci_film(sock, id_utente_loggato);
                break;
            case 2:
                return;
            default:
                printf("Scelta non valida. Riprova.\n");
                printf("\nPremi invio per continuare...");
                getchar();
        }
    }
}

void restituisci_film(int sock, int* id_utente_loggato){
    int id_film = 0;
    printf("Inserisci l'ID del film da restituire: ");

    if (scanf("%d", &id_film) != 1) {
        // Input non valido (non è un numero)
        while (getchar() != '\n'); // Pulisci il buffer di input
        id_film = 0; // Imposta una scelta non valida per entrare nel default
    } else {
        getchar(); // Consuma il carattere newline solo se l'input era valido
    }

    // Controlla se l'ID del film è valido
    if (id_film <= 0) {
        printf("ID film non valido. Riprova.\n");
        printf("\nPremi invio per continuare...");
        getchar();
        return;
    }

    // Invia la richiesta di restituzione al server
    char request[BUFFER_SIZE];
    snprintf(request, BUFFER_SIZE, "RESTITUZIONE:%d:%d", *id_utente_loggato, id_film);
    send(sock, request, strlen(request), 0);

    char buffer[BUFFER_SIZE] = {0};
    read(sock, buffer, BUFFER_SIZE);

    // Controlla la risposta del server
    if (strcmp(buffer, "RETURN_OK") == 0) {
        printf("Film con ID %d restituito con successo.\n", id_film);
    } else if (strcmp(buffer, "RETURN_FAIL") == 0) {
        printf("Errore nella restituzione del film.\n");
    } else {
        printf("Errore nella comunicazione con il server: %s\n", buffer);
    }

    printf("\nPremi invio per continuare...");
    getchar();
}