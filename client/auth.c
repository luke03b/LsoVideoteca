# include "auth.h"

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

// Funzione per effettuare il login
void do_login(int sock, int* num_film, int* id_utente_loggato, int* carrello) {
    char username[100];
    char password[100];
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE] = {0};
    
    clear_screen();
    
    printf(BOLD COLOR_CYAN "╔═════════════════════════════════╗\n" COLOR_RESET);
    printf(BOLD COLOR_CYAN "║" COLOR_WHITE "         PAGINA DI LOGIN         " COLOR_CYAN "║\n" COLOR_RESET);
    printf(BOLD COLOR_CYAN "╚═════════════════════════════════╝\n\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "Username: " COLOR_RESET);
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Rimuovi il newline
    
    printf(COLOR_YELLOW "Password: " COLOR_RESET);
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Rimuovi il newline
    
    // Crea la richiesta di login
    snprintf(request, BUFFER_SIZE, "LOGIN:%s:%s", username, password);
    
    printf(COLOR_BLUE "\n[⟳] Autenticazione in corso..." COLOR_RESET);
    fflush(stdout);
    
    // Invia la richiesta al server
    send(sock, request, strlen(request), 0);
    
    // Ricevi la risposta
    read(sock, buffer, BUFFER_SIZE);
    
    clear_screen();
    
    // Controlla la risposta del server
    if (strncmp(buffer, "AUTH_OK:", 8) == 0) {
        // Estrai l'ID utente dalla risposta
        *id_utente_loggato = atoi(buffer + 8);
        printf(BOLD COLOR_GREEN "╔═══════════════════════════════════════╗\n" COLOR_RESET);
        printf(BOLD COLOR_GREEN "║" COLOR_WHITE "     LOGIN EFFETTUATO CON SUCCESSO     " COLOR_GREEN "║\n" COLOR_RESET);
        printf(BOLD COLOR_GREEN "╚═══════════════════════════════════════╝\n\n" COLOR_RESET);
        printf(COLOR_GREEN "[✔] Login completato con successo!\n" COLOR_RESET);
        printf("\n" COLOR_YELLOW "Premi invio per continuare..." COLOR_RESET);
        getchar();
        show_main_menu(sock, username, num_film, id_utente_loggato, carrello);
    } else if (strcmp(buffer, "AUTH_FAIL") == 0) {
        *id_utente_loggato = -1;  // Assicurati che l'ID sia -1 se il login fallisce
        printf(BOLD COLOR_RED "╔════════════════════════════════╗\n" COLOR_RESET);
        printf(BOLD COLOR_RED "║" COLOR_WHITE "             ERRORE             " COLOR_RED "║\n" COLOR_RESET);
        printf(BOLD COLOR_RED "╚════════════════════════════════╝\n\n" COLOR_RESET);
        printf(COLOR_RED "[✘] Username o password non validi!\n" COLOR_RESET);
        printf("\n" COLOR_YELLOW "Premi invio per continuare..." COLOR_RESET);
        getchar();
        clear_screen();
    } else {
        *id_utente_loggato = -1;  // Assicurati che l'ID sia -1 se c'è un errore
        printf(BOLD COLOR_RED "╔════════════════════════════════╗\n" COLOR_RESET);
        printf(BOLD COLOR_RED "║" COLOR_WHITE "             ERRORE             " COLOR_RED "║\n" COLOR_RESET);
        printf(BOLD COLOR_RED "╚════════════════════════════════╝\n\n" COLOR_RESET);
        printf(COLOR_RED "[✘] Errore nella comunicazione con il server: %s\n" COLOR_RESET, buffer);
        printf("\n" COLOR_YELLOW "Premi invio per continuare..." COLOR_RESET);
        getchar();
        clear_screen();
    }
}

// Funzione per effettuare la registrazione
void do_registration(int sock) {
    char username[100];
    char password[100];
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE] = {0};
    
    clear_screen();
    
    printf(BOLD COLOR_CYAN "╔═════════════════════════════════╗\n" COLOR_RESET);
    printf(BOLD COLOR_CYAN "║" COLOR_WHITE "     PAGINA DI REGISTRAZIONE     " COLOR_CYAN "║\n" COLOR_RESET);
    printf(BOLD COLOR_CYAN "╚═════════════════════════════════╝\n\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "Username: " COLOR_RESET);
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Rimuovi il newline
    
    printf(COLOR_YELLOW "Password: " COLOR_RESET);
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Rimuovi il newline
    
    // Crea la richiesta di registrazione
    snprintf(request, BUFFER_SIZE, "REGISTRAZIONE:%s:%s", username, password);
    
    printf(COLOR_BLUE "\n[⟳] Registrazione in corso..." COLOR_RESET);
    fflush(stdout);
    
    // Invia la richiesta al server
    send(sock, request, strlen(request), 0);
    
    // Ricevi la risposta
    read(sock, buffer, BUFFER_SIZE);
    
    clear_screen();
    
    // Controlla la risposta del server
    if (strcmp(buffer, "REGISTRATION_OK") == 0) {
        printf(BOLD COLOR_GREEN "╔════════════════════════════════╗\n" COLOR_RESET);
        printf(BOLD COLOR_GREEN "║" COLOR_WHITE "     REGISTRAZIONE RIUSCITA     " COLOR_GREEN "║\n" COLOR_RESET);
        printf(BOLD COLOR_GREEN "╚════════════════════════════════╝\n\n" COLOR_RESET);
        printf(COLOR_GREEN "[✔] Registrazione completata con successo!\n" COLOR_RESET);
        printf("\n" COLOR_YELLOW "Premi invio per continuare..." COLOR_RESET);
        getchar();
        clear_screen();
    } else if (strcmp(buffer, "REGISTRATION_FAIL") == 0) {
        printf(BOLD COLOR_RED "╔════════════════════════════════╗\n" COLOR_RESET);
        printf(BOLD COLOR_RED "║" COLOR_WHITE "             ERRORE             " COLOR_RED "║\n" COLOR_RESET);
        printf(BOLD COLOR_RED "╚════════════════════════════════╝\n\n" COLOR_RESET);
        printf(COLOR_RED "[✘] Username o password non validi. Errore nella registrazione!\n" COLOR_RESET);
        printf("\n" COLOR_YELLOW "Premi invio per continuare..." COLOR_RESET);
        getchar();
        clear_screen();
    } else {
        printf(BOLD COLOR_RED "╔════════════════════════════════╗\n" COLOR_RESET);
        printf(BOLD COLOR_RED "║" COLOR_WHITE "             ERRORE             " COLOR_RED "║\n" COLOR_RESET);
        printf(BOLD COLOR_RED "╚════════════════════════════════╝\n\n" COLOR_RESET);
        printf(COLOR_RED "[✘] Errore nella comunicazione con il server: %s\n" COLOR_RESET, buffer);
        printf("\n" COLOR_YELLOW "Premi invio per continuare..." COLOR_RESET);
        getchar();
        clear_screen();
    }
}