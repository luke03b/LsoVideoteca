# include "loan.h"

// Definizione colori ANSI
#define COLOR_RESET   "\x1b[0m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_WHITE   "\x1b[37m"
#define BOLD          "\x1b[1m"
#define UNDERLINE     "\x1b[4m"
#define BG_BLACK      "\x1b[40m"
#define BG_BLUE       "\x1b[44m"

void add_to_cart(int sock, int* num_film, int* carrello){
    int id_film = 0;
    printf(COLOR_YELLOW "\n[🛒] Inserisci l'ID del film da aggiungere al carrello: " COLOR_RESET);

    if (scanf("%d", &id_film) != 1) {
        // Input non valido (non è un numero)
        while (getchar() != '\n'); // Pulisci il buffer di input
        id_film = 0; // Imposta una scelta non valida per entrare nel default
    } else {
        getchar(); // Consuma il carattere newline solo se l'input era valido
    }

    // Controlla se l'ID del film è valido
    if (id_film <= 0) {
        printf(COLOR_RED "\n[✘] ID film non valido. Riprova.\n" COLOR_RESET);
        printf("\nPremi invio per continuare...");
        getchar();
        return;
    }

    // Controlla se il film è già presente nel carrello
    if (is_film_already_in_cart(id_film, num_film, carrello)) {
        printf(COLOR_YELLOW "\n[⚠️] Il film con ID %d è già presente nel carrello.\n" COLOR_RESET, id_film);
        printf("\nPremi invio per continuare...");
        getchar();
        return;
    }

    // Controlla se il carrello è pieno
    if (*num_film < 5) {
        carrello[*num_film] = id_film;
        (*num_film)++;
        printf(COLOR_GREEN "\n[✔] Film con ID %d aggiunto al carrello.\n" COLOR_RESET, id_film);
    } else {
        printf(COLOR_RED "\n[✘] Carrello pieno. Non puoi aggiungere più di 5 film.\n" COLOR_RESET);
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
        printf(BOLD COLOR_CYAN "╔════════════════════════════════╗\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "║" COLOR_WHITE "          CARRELLO              " COLOR_CYAN "║\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "╚════════════════════════════════╝\n\n" COLOR_RESET);
        
        if (*num_film <= 0) {
            printf(COLOR_YELLOW "[📭] Nessun film nel carrello.\n" COLOR_RESET);
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
            printf(COLOR_RED "[✘] Errore nella ricezione dei dati dal server.\n" COLOR_RESET);
            printf("\nPremi invio per continuare...");
            getchar();
            return;
        }
        
        buffer[bytes_received] = '\0';
        
        if (strncmp(buffer, "SEARCH_FAIL", 11) == 0) {
            printf(COLOR_RED "[✘] Nessun film trovato con gli ID specificati.\n" COLOR_RESET);
        } else {
            printf(COLOR_GREEN "[🎬] Film trovati nel carrello:\n\n" COLOR_RESET);
            display_films(buffer, 1);
        }


        int choice = 0;
        // Mostra le opzioni del carrello
        printf(COLOR_BLUE "\n╔════════════════════════════════╗\n" COLOR_RESET);
        printf(COLOR_BLUE "║" COLOR_WHITE BOLD "          OPZIONI               " COLOR_BLUE "║\n" COLOR_RESET);
        printf(COLOR_BLUE "╠════════════════════════════════╣\n" COLOR_RESET);
        printf(COLOR_BLUE "║" COLOR_RESET " 💰 " COLOR_WHITE "1. Effettua il Check-out    " COLOR_BLUE "║\n" COLOR_RESET);
        printf(COLOR_BLUE "║" COLOR_RESET " 🗑️ " COLOR_WHITE "2. Svuota Carrello          " COLOR_BLUE "║\n" COLOR_RESET);
        printf(COLOR_BLUE "║" COLOR_RESET " 🏠 " COLOR_WHITE "3. Torna al menu principale " COLOR_BLUE "║\n" COLOR_RESET);
        printf(COLOR_BLUE "╚════════════════════════════════╝\n" COLOR_RESET);
        
        printf(COLOR_YELLOW "\nScelta: " COLOR_RESET);
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
                printf(COLOR_GREEN "\n[✔] Carrello svuotato.\n" COLOR_RESET);
                printf("\nPremi invio per continuare...");
                getchar();
                break;
            case 3:
                return;
            default:
                printf(COLOR_RED "\n[✘] Scelta non valida. Riprova.\n" COLOR_RESET);
                printf("\nPremi invio per continuare...");
                getchar();
        
        }
    }
}

void check_out(int sock, int* num_film, int* id_utente_loggato, int* carrello){
    printf(COLOR_CYAN "\n[⟳] Elaborazione noleggio in corso...\n" COLOR_RESET);
    
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
            printf(COLOR_GREEN "[✔] Hai noleggiato il film con ID %d\n" COLOR_RESET, carrello[i]);
        } else if (strcmp(buffer, "LOAN_FAIL") == 0) {
            printf(COLOR_RED "[✘] Errore nel noleggio del film con ID %d\n" COLOR_RESET, carrello[i]);
        } else {
            printf(COLOR_RED "[✘] Errore nella comunicazione con il server: %s\n" COLOR_RESET, buffer);
        }
    }
    *num_film = 0; // Svuota il carrello dopo il check-out
    printf(COLOR_GREEN "\n[✔] Carrello svuotato dopo il check-out.\n" COLOR_RESET);
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
        printf(BOLD COLOR_MAGENTA "╔═════════════════════════════╗\n" COLOR_RESET);
        printf(BOLD COLOR_MAGENTA "║" COLOR_WHITE "       FILM NOLEGGIATI       " COLOR_MAGENTA "║\n" COLOR_RESET);
        printf(BOLD COLOR_MAGENTA "╚═════════════════════════════╝\n\n" COLOR_RESET);
        
        char request[BUFFER_SIZE];
        char buffer[BUFFER_SIZE * 20] = {0};
        int bytes_received;
        
        printf(COLOR_BLUE "[⟳] Recupero dei film noleggiati..." COLOR_RESET);
        
        // Crea la richiesta di ricerca (5 = ricerca film noleggiati utente loggato)
        snprintf(request, BUFFER_SIZE, "SEARCH:5:%d", *id_utente_loggato);
        send(sock, request, strlen(request), 0);
        
        bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        
        printf("\r                                         \r"); // Pulisce la riga del caricamento
        
        if (bytes_received <= 0) {
            printf(COLOR_RED "[✘] Errore nella ricezione dei dati dal server.\n" COLOR_RESET);
            printf("\nPremi invio per continuare...");
            getchar();
            return;
        }
        
        buffer[bytes_received] = '\0';
        
        if (strncmp(buffer, "SEARCH_FAIL", 11) == 0) {
            printf(COLOR_YELLOW "[📭] Nessun film noleggiato\n" COLOR_RESET);
            printf("\nPremi invio per tornare al menu principale...");
            getchar();
            return;
        } else {
            printf(COLOR_GREEN "[🎬] Film attualmente noleggiati:\n\n" COLOR_RESET);
            display_films(buffer, 0);
        }
    
        int choice = 0;
        // Mostra le opzioni del carrello
        printf(COLOR_MAGENTA "\n╔════════════════════════════════╗\n" COLOR_RESET);
        printf(COLOR_MAGENTA "║" COLOR_WHITE BOLD "          OPZIONI               " COLOR_MAGENTA "║\n" COLOR_RESET);
        printf(COLOR_MAGENTA "╠════════════════════════════════╣\n" COLOR_RESET);
        printf(COLOR_MAGENTA "║" COLOR_RESET " 📦 " COLOR_WHITE "1. Restituisci film         " COLOR_MAGENTA "║\n" COLOR_RESET);
        printf(COLOR_MAGENTA "║" COLOR_RESET " 🏠 " COLOR_WHITE "2. Torna al menu principale " COLOR_MAGENTA "║\n" COLOR_RESET);
        printf(COLOR_MAGENTA "╚════════════════════════════════╝\n" COLOR_RESET);
        
        printf(COLOR_YELLOW "\nScelta: " COLOR_RESET);

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
                printf(COLOR_RED "\n[✘] Scelta non valida. Riprova.\n" COLOR_RESET);
                printf("\nPremi invio per continuare...");
                getchar();
        }
    }
}

void restituisci_film(int sock, int* id_utente_loggato){
    int id_film = 0;
    printf(COLOR_YELLOW "\n[📦] Inserisci l'ID del film da restituire: " COLOR_RESET);

    if (scanf("%d", &id_film) != 1) {
        // Input non valido (non è un numero)
        while (getchar() != '\n'); // Pulisci il buffer di input
        id_film = 0; // Imposta una scelta non valida per entrare nel default
    } else {
        getchar(); // Consuma il carattere newline solo se l'input era valido
    }

    // Controlla se l'ID del film è valido
    if (id_film <= 0) {
        printf(COLOR_RED "\n[✘] ID film non valido. Riprova.\n" COLOR_RESET);
        printf("\nPremi invio per continuare...");
        getchar();
        return;
    }

    printf(COLOR_CYAN "\n[⟳] Elaborazione restituzione in corso..." COLOR_RESET);
    fflush(stdout);

    // Invia la richiesta di restituzione al server
    char request[BUFFER_SIZE];
    snprintf(request, BUFFER_SIZE, "RESTITUZIONE:%d:%d", *id_utente_loggato, id_film);
    send(sock, request, strlen(request), 0);

    char buffer[BUFFER_SIZE] = {0};
    read(sock, buffer, BUFFER_SIZE);

    printf("\r                                           \r"); // Pulisce la riga del caricamento

    // Controlla la risposta del server
    if (strcmp(buffer, "RETURN_OK") == 0) {
        printf(COLOR_GREEN "\n[✔] Film con ID %d restituito con successo.\n" COLOR_RESET, id_film);
    } else if (strcmp(buffer, "RETURN_FAIL") == 0) {
        printf(COLOR_RED "\n[✘] Errore nella restituzione del film.\n" COLOR_RESET);
    } else {
        printf(COLOR_RED "\n[✘] Errore nella comunicazione con il server: %s\n" COLOR_RESET, buffer);
    }

    printf("\nPremi invio per continuare...");
    getchar();
}