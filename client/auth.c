# include "auth.h"

// Funzione per effettuare il login
void do_login(int sock, int* num_film, int* id_utente_loggato, int* carrello) {
    char username[100];
    char password[100];
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE] = {0};
    
    clear_screen();
    printf("===== PAGINA DI LOGIN =====\n");
    
    printf("Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Rimuovi il newline
    
    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Rimuovi il newline
    
    // Crea la richiesta di login
    snprintf(request, BUFFER_SIZE, "LOGIN:%s:%s", username, password);
    
    // Invia la richiesta al server
    send(sock, request, strlen(request), 0);
    
    // Ricevi la risposta
    read(sock, buffer, BUFFER_SIZE);
    
    clear_screen();
    
    // Controlla la risposta del server
    if (strncmp(buffer, "AUTH_OK:", 8) == 0) {
        // Estrai l'ID utente dalla risposta
        *id_utente_loggato = atoi(buffer + 8);
        show_main_menu(sock, username, num_film, id_utente_loggato, carrello);
    } else if (strcmp(buffer, "AUTH_FAIL") == 0) {
        *id_utente_loggato = -1;  // Assicurati che l'ID sia -1 se il login fallisce
        printf("===== ERRORE =====\n");
        printf("Username o password non validi!\n");
        printf("\nPremi invio per continuare...");
        getchar();
        clear_screen();
    } else {
        *id_utente_loggato = -1;  // Assicurati che l'ID sia -1 se c'è un errore
        printf("===== ERRORE =====\n");
        printf("Errore nella comunicazione con il server: %s\n", buffer);
        printf("\nPremi invio per continuare...");
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
    printf("===== PAGINA DI REGISTRAZIONE =====\n");
    
    printf("Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Rimuovi il newline
    
    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Rimuovi il newline
    
    // Crea la richiesta di registrazione
    snprintf(request, BUFFER_SIZE, "REGISTRAZIONE:%s:%s", username, password);
    
    // Invia la richiesta al server
    send(sock, request, strlen(request), 0);
    
    // Ricevi la risposta
    read(sock, buffer, BUFFER_SIZE);
    
    clear_screen();
    
    // Controlla la risposta del server
    if (strcmp(buffer, "REGISTRATION_OK") == 0) {
        printf("===== REGISTRAZIONE RIUSCITA =====\n");
        printf("Ho effettuato la registrazione\n");
        printf("\nPremi invio per continuare...");
        getchar();
        clear_screen();
    } else if (strcmp(buffer, "REGISTRATION_FAIL") == 0) {
        printf("===== ERRORE =====\n");
        printf("Username o password non validi. Errore nella registrazione!\n");
        printf("\nPremi invio per continuare...");
        getchar();
        clear_screen();
    } else {
        printf("===== ERRORE =====\n");
        printf("Errore nella comunicazione con il server: %s\n", buffer);
        printf("\nPremi invio per continuare...");
        getchar();
        clear_screen();
    }
}