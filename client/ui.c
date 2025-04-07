# include "ui.h"

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

// Funzione per pulire lo schermo
void clear_screen() {
    printf("\033[H\033[J");
}

void show_main_menu(int sock, char *username, int* num_film, int* id_utente_loggato, int* carrello) {
    int choice = 0;
    // Loop principale del menu
    while (1) {
        clear_screen();
        display_logo();
        
        printf(BOLD COLOR_CYAN "╔════════════════════════════════╗\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "║" COLOR_GREEN "  BENVENUTO, %-19s" COLOR_CYAN "║\n" COLOR_RESET, username);
        printf(BOLD COLOR_CYAN "╠════════════════════════════════╣\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "║" COLOR_RESET " 🎞️  " COLOR_WHITE "1. Noleggia Film           " COLOR_CYAN "║\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "║" COLOR_RESET " 🛒 " COLOR_WHITE "2. Carrello                 " COLOR_CYAN "║\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "║" COLOR_RESET " 📋 " COLOR_WHITE "3. Film noleggiati          " COLOR_CYAN "║\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "║" COLOR_RESET " 🔔 " COLOR_WHITE "4. Notifiche                " COLOR_CYAN "║\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "║" COLOR_RESET " 🚪 " COLOR_WHITE "5. Logout                   " COLOR_CYAN "║\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "╚════════════════════════════════╝\n" COLOR_RESET);
        
        if (*num_film > 0) {
            printf(COLOR_YELLOW "\n[🛒] Hai %d film nel carrello\n" COLOR_RESET, *num_film);
        }
        
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
                show_search_menu(sock, num_film, carrello);
                break;
            case 2:
                show_cart(sock, num_film, id_utente_loggato, carrello);
                break;
            case 3:
                retrieve_loaned_films(sock, id_utente_loggato);
                break;
            case 4:
                show_notifications(sock, id_utente_loggato);
                break;
            case 5:
                printf(COLOR_GREEN "\n[✔] Logout effettuato. Arrivederci %s!\n" COLOR_RESET, username);
                printf("\nPremi invio per continuare...");
                getchar();
                clear_screen();
                return;
            default:
                printf(COLOR_RED "\n[✘] Scelta non valida. Riprova.\n" COLOR_RESET);
                printf("\nPremi invio per continuare...");
                getchar();
        }
    }
}

// Funzione per richiedere e visualizzare il catalogo film
void view_catalogo(int sock) {
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 10] = {0};
    int bytes_received;
    
    clear_screen();
    printf(COLOR_CYAN "╔══════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_WHITE BOLD "          CATALOGO FILM               " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "╚══════════════════════════════════════╝\n\n" COLOR_RESET);
    
    printf(COLOR_BLUE "[⟳] Caricamento catalogo..." COLOR_RESET);
    fflush(stdout);
    
    snprintf(request, BUFFER_SIZE, "CATALOGO");
    send(sock, request, strlen(request), 0);
    
    bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    
    printf("\r                           \r"); // Pulisce la riga del caricamento
    
    if (bytes_received <= 0) {
        printf(COLOR_RED "[✘] Errore nella ricezione dei dati dal server.\n" COLOR_RESET);
        printf("\nPremi invio per continuare...");
        getchar(); // Attende che l'utente prema invio
        return;
    }
    
    buffer[bytes_received] = '\0';
    
    if (strncmp(buffer, "FILM_FAIL", 9) == 0) {
        printf(COLOR_RED "[✘] Errore: Impossibile recuperare il catalogo film.\n" COLOR_RESET);
    } else {
        display_films(buffer, 1);
    }
}

void display_films(const char *film_data, int options) {
    char *data = strdup(film_data);
    if (data == NULL) {
        printf(COLOR_RED "[✘] Errore: memoria insufficiente.\n" COLOR_RESET);
        return;
    }
    
    char *token;
    char *rest = data;
    int film_count = 0;
    
    token = strtok_r(rest, "\n", &rest);
    if (token != NULL) {
        film_count = atoi(token);
        printf(COLOR_GREEN "[🎬] Trovati %d film:\n\n" COLOR_RESET, film_count);
    }

    if (options == 1) {
        // Intestazione della tabella per catalogo normale
        printf(COLOR_CYAN "+-----+---------------------------+------------------+-------------------+\n");
        printf("| " COLOR_YELLOW "%-3s" COLOR_CYAN " | " COLOR_YELLOW "%-25s" COLOR_CYAN " | " COLOR_YELLOW "%-16s" COLOR_CYAN " | " COLOR_YELLOW "%-17s" COLOR_CYAN " |\n", 
               "ID", "TITOLO", "GENERE", "COPIE DISPONIBILI");
        printf("+-----+---------------------------+------------------+-------------------+\n" COLOR_RESET);
    } else {
        // Intestazione della tabella per film noleggiati
        printf(COLOR_MAGENTA "+-----+---------------------------+------------------+-------------------+\n");
        printf("| " COLOR_YELLOW "%-3s" COLOR_MAGENTA " | " COLOR_YELLOW "%-25s" COLOR_MAGENTA " | " COLOR_YELLOW "%-16s" COLOR_MAGENTA " | " COLOR_YELLOW "%-17s" COLOR_MAGENTA " |\n", 
               "ID", "TITOLO", "GENERE", "DATA RESTITUZIONE");
        printf("+-----+---------------------------+------------------+-------------------+\n" COLOR_RESET);
    }
    
    char id[10] = "";
    char titolo[100] = "";
    char genere[50] = "";
    char ultima_colonna[11] = "";
    int row_count = 0;
    
    while ((token = strtok_r(rest, "\n", &rest)) != NULL) {
        if (strcmp(token, "---") == 0) {
            // Stampa il film corrente in formato tabella
            if (titolo[0] != '\0') {
                // Alterna i colori delle righe per maggiore leggibilità
                if (row_count % 2 == 0) {
                    printf(options == 1 ? COLOR_CYAN : COLOR_MAGENTA);
                } else {
                    printf(COLOR_RESET);
                }
                
                printf("| %-3s | %-25s | %-16s | %-17s |\n", 
                       id,
                       titolo, 
                       genere, 
                       ultima_colonna);
                
                row_count++;
                
                // Resetta i campi per il prossimo film
                id[0] = '\0';
                titolo[0] = '\0';
                genere[0] = '\0';
                ultima_colonna[0] = '\0';
            }
            continue;
        }
        
        char *key = token;
        char *value = strchr(token, ':');
        
        if (value != NULL) {
            *value = '\0';  // Termina la chiave
            value += 2;     // Salta il ':' e lo spazio
            
            if (strcasecmp(key, "id") == 0) {
                strncpy(id, value, sizeof(id) - 1);
                id[sizeof(id) - 1] = '\0';
            } else if (strcasecmp(key, "titolo") == 0) {
                strncpy(titolo, value, sizeof(titolo) - 1);
                titolo[sizeof(titolo) - 1] = '\0';
            } else if (strcasecmp(key, "genere") == 0) {
                strncpy(genere, value, sizeof(genere) - 1);
                genere[sizeof(genere) - 1] = '\0';
            } else if (options == 1 && strcasecmp(key, "copie_disponibili") == 0) {
                strncpy(ultima_colonna, value, sizeof(ultima_colonna) - 1);
                ultima_colonna[sizeof(ultima_colonna) - 1] = '\0';
            } else if (options == 0 && strcasecmp(key, "data_restituzione") == 0) {
                strncpy(ultima_colonna, value, sizeof(ultima_colonna) - 1);
                ultima_colonna[sizeof(ultima_colonna) - 1] = '\0';
            }
        }
    }
    
    // Stampa l'ultimo film se c'è
    if (titolo[0] != '\0') {
        if (row_count % 2 == 0) {
            printf(options == 1 ? COLOR_CYAN : COLOR_MAGENTA);
        } else {
            printf(COLOR_RESET);
        }
        
        printf("| %-3s | %-25s | %-16s | %-17s |\n", 
               id, 
               titolo, 
               genere, 
               ultima_colonna);
    }
    
    // Chiudi la tabella
    printf(options == 1 ? COLOR_CYAN : COLOR_MAGENTA);
    printf("+-----+---------------------------+------------------+-------------------+\n");
    printf(COLOR_RESET);
    
    free(data);
}

// Funzione per mostrare il menu di ricerca film
void show_search_menu(int sock, int* num_film, int* carrello) {
    int choice = 0;
    
    while (1) {
        clear_screen();
        view_catalogo(sock);
        
        printf(COLOR_BLUE "\n╔══════════════════════════════════╗\n" COLOR_RESET);
        printf(COLOR_BLUE "║" COLOR_WHITE BOLD "      MENU RICERCA FILM           " COLOR_BLUE "║\n" COLOR_RESET);
        printf(COLOR_BLUE "╠══════════════════════════════════╣\n" COLOR_RESET);
        printf(COLOR_BLUE "║" COLOR_RESET " 🛒 " COLOR_WHITE "1. Aggiungi al carrello       " COLOR_BLUE "║\n" COLOR_RESET);
        printf(COLOR_BLUE "║" COLOR_RESET " 🔍 " COLOR_WHITE "2. Cerca per titolo           " COLOR_BLUE "║\n" COLOR_RESET);
        printf(COLOR_BLUE "║" COLOR_RESET " 📚 " COLOR_WHITE "3. Cerca per genere           " COLOR_BLUE "║\n" COLOR_RESET);
        printf(COLOR_BLUE "║" COLOR_RESET " 🌟 " COLOR_WHITE "4. Mostra i film più popolari " COLOR_BLUE "║\n" COLOR_RESET);
        printf(COLOR_BLUE "║" COLOR_RESET " 🏠 " COLOR_WHITE "5. Torna al menu principale   " COLOR_BLUE "║\n" COLOR_RESET);
        printf(COLOR_BLUE "╚══════════════════════════════════╝\n" COLOR_RESET);
        
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
                add_to_cart(sock, num_film, carrello);
                break;
            case 2:
                search_by_title(sock);
                break;
            case 3:
                search_by_genre(sock);
                break;
            case 4:
                search_by_popularity(sock);
                break;
            case 5:
                return;
            default:
                printf(COLOR_RED "\n[✘] Scelta non valida. Riprova.\n" COLOR_RESET);
                printf("\nPremi invio per continuare...");
                getchar();
        }
    }
}