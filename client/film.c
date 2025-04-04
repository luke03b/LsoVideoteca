# include "film.h"

// Funzione per cercare film per titolo
void search_by_title(int sock) {
    char title[100];
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 20] = {0};
    int bytes_received;
    
    clear_screen();
    printf("===== RICERCA FILM PER TITOLO =====\n\n");
    
    printf("Inserisci parte del titolo: ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = 0; // Rimuovi il newline
    
    // Crea la richiesta di ricerca (1 = ricerca per titolo)
    snprintf(request, BUFFER_SIZE, "SEARCH:1:%s", title);
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
        printf("Nessun film trovato con il titolo '%s'.\n", title);
    } else {
        printf("Risultati della ricerca per titolo '%s':\n\n", title);
        display_films(buffer, 1);
    }
    
    printf("\nPremi invio per tornare al menu principale...");
    getchar();
}

// Funzione per cercare film per genere
void search_by_genre(int sock) {
    char genre[100];
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 20] = {0};
    int bytes_received;
    
    clear_screen();
    printf("===== RICERCA FILM PER GENERE =====\n\n");
    
    printf("Inserisci genere: ");
    fgets(genre, sizeof(genre), stdin);
    genre[strcspn(genre, "\n")] = 0; // Rimuovi il newline
    
    // Crea la richiesta di ricerca (2 = ricerca per genere)
    snprintf(request, BUFFER_SIZE, "SEARCH:2:%s", genre);
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
        printf("Nessun film trovato nel genere '%s'.\n", genre);
    } else {
        printf("Risultati della ricerca per genere '%s':\n\n", genre);
        display_films(buffer, 1);
    }
    
    printf("\nPremi invio per tornare al menu principale...");
    getchar();
}

// Funzione per cercare film più popolari
void search_by_popularity(int sock) {
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 20] = {0};
    int bytes_received;
    
    clear_screen();
    printf("===== FILM PIÙ POPOLARI =====\n\n");
    
    // Crea la richiesta di ricerca (3 = ricerca per popolarità)
    // Non è necessario passare una query, quindi usiamo un valore vuoto
    snprintf(request, BUFFER_SIZE, "SEARCH:3:");
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
        printf("Errore nel recupero dei film più popolari.\n");
    } else {
        printf("Film più popolari (ordinati per numero di noleggi):\n\n");
        display_films(buffer, 1);
    }
    
    printf("\nPremi invio per tornare al menu principale...");
    getchar();
}