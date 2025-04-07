# include "film.h"

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

// Funzione per cercare film per titolo
void search_by_title(int sock) {
    char title[100];
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 20] = {0};
    int bytes_received;
    
    clear_screen();
    printf(COLOR_CYAN "╔═══════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_WHITE BOLD "      RICERCA FILM PER TITOLO      " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "╚═══════════════════════════════════╝\n\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "Inserisci parte del titolo: " COLOR_RESET);
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = 0; // Rimuovi il newline
    
    // Crea la richiesta di ricerca (1 = ricerca per titolo)
    snprintf(request, BUFFER_SIZE, "SEARCH:1:%s", title);
    send(sock, request, strlen(request), 0);
    
    printf(COLOR_BLUE "\n[⟳] Ricerca in corso..." COLOR_RESET);
    fflush(stdout);
    
    bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    
    printf("\r                         \r"); // Pulisce la riga del caricamento
    
    if (bytes_received <= 0) {
        printf(COLOR_RED "\n[✘] Errore nella ricezione dei dati dal server.\n" COLOR_RESET);
        printf("\nPremi invio per continuare...");
        getchar();
        return;
    }
    
    buffer[bytes_received] = '\0';
    
    if (strncmp(buffer, "SEARCH_FAIL", 11) == 0) {
        printf(COLOR_RED "\n[✘] Nessun film trovato con il titolo '" COLOR_YELLOW "%s" COLOR_RED "'.\n" COLOR_RESET, title);
    } else {
        printf(COLOR_GREEN "\n[✔] Risultati della ricerca per titolo '" COLOR_YELLOW "%s" COLOR_GREEN "':\n\n" COLOR_RESET, title);
        display_films(buffer, 1);
    }
    
    printf(COLOR_CYAN "\nPremi invio per tornare al menu principale..." COLOR_RESET);
    getchar();
}

// Funzione per cercare film per genere
void search_by_genre(int sock) {
    char genre[100];
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 20] = {0};
    int bytes_received;
    
    clear_screen();
    printf(COLOR_CYAN "╔═══════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_WHITE BOLD "      RICERCA FILM PER GENERE      " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "╚═══════════════════════════════════╝\n\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "Inserisci genere: " COLOR_RESET);
    fgets(genre, sizeof(genre), stdin);
    genre[strcspn(genre, "\n")] = 0; // Rimuovi il newline
    
    // Crea la richiesta di ricerca (2 = ricerca per genere)
    snprintf(request, BUFFER_SIZE, "SEARCH:2:%s", genre);
    send(sock, request, strlen(request), 0);
    
    printf(COLOR_BLUE "\n[⟳] Ricerca in corso..." COLOR_RESET);
    fflush(stdout);
    
    bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    
    printf("\r                         \r"); // Pulisce la riga del caricamento
    
    if (bytes_received <= 0) {
        printf(COLOR_RED "\n[✘] Errore nella ricezione dei dati dal server.\n" COLOR_RESET);
        printf("\nPremi invio per continuare...");
        getchar();
        return;
    }
    
    buffer[bytes_received] = '\0';
    
    if (strncmp(buffer, "SEARCH_FAIL", 11) == 0) {
        printf(COLOR_RED "\n[✘] Nessun film trovato nel genere '" COLOR_YELLOW "%s" COLOR_RED "'.\n" COLOR_RESET, genre);
    } else {
        printf(COLOR_GREEN "\n[✔] Risultati della ricerca per genere '" COLOR_YELLOW "%s" COLOR_GREEN "':\n\n" COLOR_RESET, genre);
        display_films(buffer, 1);
    }
    
    printf(COLOR_CYAN "\nPremi invio per tornare al menu principale..." COLOR_RESET);
    getchar();
}

// Funzione per cercare film più popolari
void search_by_popularity(int sock) {
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 20] = {0};
    int bytes_received;
    
    clear_screen();
    printf(COLOR_CYAN "╔═════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_WHITE BOLD "          FILM PIÙ POPOLARI          " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "╚═════════════════════════════════════╝\n\n" COLOR_RESET);
    
    // Crea la richiesta di ricerca (3 = ricerca per popolarità)
    // Non è necessario passare una query, quindi usiamo un valore vuoto
    snprintf(request, BUFFER_SIZE, "SEARCH:3:");
    send(sock, request, strlen(request), 0);
    
    printf(COLOR_BLUE "[⟳] Caricamento classifica..." COLOR_RESET);
    fflush(stdout);
    
    bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    
    printf("\r                              \r"); // Pulisce la riga del caricamento
    
    if (bytes_received <= 0) {
        printf(COLOR_RED "\n[✘] Errore nella ricezione dei dati dal server.\n" COLOR_RESET);
        printf("\nPremi invio per continuare...");
        getchar();
        return;
    }
    
    buffer[bytes_received] = '\0';
    
    if (strncmp(buffer, "SEARCH_FAIL", 11) == 0) {
        printf(COLOR_RED "\n[✘] Errore nel recupero dei film più popolari.\n" COLOR_RESET);
    } else {
        printf(COLOR_GREEN "\n[✔] Film più popolari " COLOR_YELLOW "(ordinati per numero di noleggi)" COLOR_GREEN ":\n\n" COLOR_RESET);
        display_films(buffer, 1);
    }
    
    printf(COLOR_CYAN "\nPremi invio per tornare al menu principale..." COLOR_RESET);
    getchar();
}