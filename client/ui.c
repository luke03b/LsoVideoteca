# include "ui.h"

// Funzione per pulire lo schermo
void clear_screen() {
    printf("\033[H\033[J");
}

void show_main_menu(int sock, char *username, int* num_film, int* id_utente_loggato, int* carrello) {
    int choice = 0;
    // Loop principale del menu
    while (1) {
        clear_screen();
        printf("===== BENVENUTO =====\n");
        printf("1. Noleggia Film\n");
        printf("2. Carrello\n");
        printf("3. Film noleggiati\n");
        printf("4. Notifiche\n");
        printf("5. Logout\n");
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
                printf("Arrivederci\n");
                printf("\nPremi invio per continuare...");
                getchar();
                clear_screen();
                return;
            default:
                printf("Scelta non valida. Riprova.\n");
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
    printf("===== CATALOGO FILM =====\n\n");
    
    snprintf(request, BUFFER_SIZE, "CATALOGO");
    send(sock, request, strlen(request), 0);
    
    bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received <= 0) {
        printf("Errore nella ricezione dei dati dal server.\n");
        printf("\nPremi invio per continuare...");
        getchar(); // Attende che l'utente prema invio
        return;
    }
    
    buffer[bytes_received] = '\0';
    
    if (strncmp(buffer, "FILM_FAIL", 9) == 0) {
        printf("Errore: Impossibile recuperare il catalogo film.\n");
    } else {
        display_films(buffer, 1);
    }
}

void display_films(const char *film_data, int options) {
    char *data = strdup(film_data);
    if (data == NULL) {
        printf("Errore: memoria insufficiente.\n");
        return;
    }
    
    char *token;
    char *rest = data;
    int film_count = 0;
    
    token = strtok_r(rest, "\n", &rest);
    if (token != NULL) {
        film_count = atoi(token);
        printf("Trovati %d film:\n\n", film_count);
    }

    if (options == 1)
    {
         // Intestazione della tabella
        printf("+-----+---------------------------+------------------+-------------------+\n");
        printf("| %-3s | %-25s | %-16s | %-17s |\n", "ID", "TITOLO", "GENERE", "COPIE DISPONIBILI");
        printf("+-----+---------------------------+------------------+-------------------+\n");
    }
    else
    {
        printf("+-----+---------------------------+------------------+-------------------+\n");
        printf("| %-3s | %-25s | %-16s | %-17s |\n", "ID", "TITOLO", "GENERE", "DATA RESTITUZIONE");
        printf("+-----+---------------------------+------------------+-------------------+\n");
    }
    
    char id[10] = "";
    char titolo[100] = "";
    char genere[50] = "";
    char ultima_colonna[11] = "";
    
    while ((token = strtok_r(rest, "\n", &rest)) != NULL) {
        if (strcmp(token, "---") == 0) {
            // Stampa il film corrente in formato tabella
            if (titolo[0] != '\0') {
                printf("| %-3s | %-25s | %-16s | %-17s |\n", 
                       id,
                       titolo, 
                       genere, 
                       ultima_colonna);
                
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
        printf("| %-3s | %-25s | %-16s | %-17s |\n", 
               id, 
               titolo, 
               genere, 
               ultima_colonna);
    }
    
    // Chiudi la tabella
    printf("+-----+---------------------------+------------------+-------------------+\n");
    
    free(data);
}

// Funzione per mostrare il menu di ricerca film
void show_search_menu(int sock, int* num_film, int* carrello) {
    int choice = 0;
    
    while (1) {
        clear_screen();
        view_catalogo(sock);
        printf("\n===== MENU RICERCA FILM =====\n");
        printf("1. Aggiungi al carrello tramite ID\n");
        printf("2. Cerca per titolo\n");
        printf("3. Cerca per genere\n");
        printf("4. Mostra i film più popolari\n");
        printf("5. Torna al menu principale\n");
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
                printf("Scelta non valida. Riprova.\n");
                printf("\nPremi invio per continuare...");
                getchar();
        }
    }
}