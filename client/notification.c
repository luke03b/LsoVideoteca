# include "notification.h"

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

void show_notifications(int sock, int* id_utente_loggato) {
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 20] = {0};
    clear_screen();
    
    printf(BOLD COLOR_YELLOW "╔═══════════════════════════╗\n" COLOR_RESET);
    printf(BOLD COLOR_YELLOW "║" COLOR_WHITE "         NOTIFICHE         " COLOR_YELLOW "║\n" COLOR_RESET);
    printf(BOLD COLOR_YELLOW "╚═══════════════════════════╝\n\n" COLOR_RESET);

    printf(COLOR_BLUE "[⟳] Verifica notifiche in corso..." COLOR_RESET);
    fflush(stdout);

    snprintf(request, BUFFER_SIZE, "NOTIFICHE:%d", *id_utente_loggato);
    send(sock, request, strlen(request), 0);
    read(sock, buffer, BUFFER_SIZE);
    
    printf("\r                                    \r"); // Pulisce la riga del caricamento
    
    // Controlla la risposta del server
    if (strcmp(buffer, "NOTIFICATION_OK") == 0) {
        printf(COLOR_RED "[⚠️] ATTENZIONE: " COLOR_YELLOW "Hai dei film da restituire oltre la data consentita\n" COLOR_RESET);
        
        printf(BOLD COLOR_YELLOW "\n╔═════════════════════════════════════════════════════════════╗\n" COLOR_RESET);
        printf(BOLD COLOR_YELLOW "║" COLOR_WHITE " Ti ricordiamo di restituire i film in ritardo al più presto " COLOR_YELLOW "║\n" COLOR_RESET);
        printf(BOLD COLOR_YELLOW "╚═════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
        
        printf("\nPremi invio per continuare...");
        getchar();
    } else if (strcmp(buffer, "NOTIFICATION_FAIL") == 0) {
        printf(COLOR_GREEN "[✔] Non hai nuove notifiche\n" COLOR_RESET);
        
        printf(BOLD COLOR_CYAN "\n╔════════════════════════════════════════════════════════╗\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "║" COLOR_WHITE " Tutti i tuoi film sono in regola con le scadenze!      " COLOR_CYAN "║\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "╚════════════════════════════════════════════════════════╝\n" COLOR_RESET);
        
        printf("\nPremi invio per continuare...");
        getchar();
    } else {
        printf(COLOR_RED "[✘] ERRORE: Impossibile comunicare con il server\n" COLOR_RESET);
        printf(COLOR_RED "[✘] Messaggio ricevuto: %s\n" COLOR_RESET, buffer);
        printf("\nPremi invio per continuare...");
        getchar();
    }
}