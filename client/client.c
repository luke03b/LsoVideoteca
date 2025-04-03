#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

// Funzione per effettuare il login
void do_login(int sock);

// Funzione per effettuare la registrazione
void do_registration(int sock);

// Funzione per mostrare il menu principale dopo il login
void show_main_menu(int sock, char *username);

// Funzione per ricevere il catalogo
void view_catalogo(int sock);

// Funzione per visualizzare i film in un formato leggibile
void display_films(const char *film_data);

// Funzione per pulire lo schermo
void clear_screen() {
    printf("\033[H\033[J");
}

int main() {
    clear_screen();
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    
    // Creazione socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nErrore nella creazione del socket\n");
        return -1;
    }
    
    // Configurazione dell'indirizzo del server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Conversione indirizzo IP da testo a binario
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        printf("\nIndirizzo IP non valido / non supportato\n");
        return -1;
    }
    
    // Tentativo di connessione al server
    printf("Tentativo di connessione al server %s:%d...\n", SERVER_IP, PORT);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnessione fallita\n");
        return -1;
    }
    
    // Leggi messaggio di benvenuto dal server
    read(sock, buffer, BUFFER_SIZE);
    printf("Server: %s\n\n", buffer);
    
    int choice = 0;
    
    // Loop principale del menu
    while (1) {
        printf("===== MENU AUTENTICAZIONE =====\n");
        printf("1. Login\n");
        printf("2. Registrazione\n");
        printf("3. Esci\n");
        printf("Scelta: ");
        scanf("%d", &choice);
        getchar(); // Consuma il carattere newline
        
        switch (choice) {
            case 1:
                do_login(sock);
                break;
            case 2:
                do_registration(sock);
                break;
            case 3:
                close(sock);
                printf("Disconnessione dal server. Arrivederci!\n");
                return 0;
            default:
                printf("Scelta non valida. Riprova.\n");
        }
    }
    
    return 0;
}

// Funzione per effettuare il login
void do_login(int sock) {
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
    if (strcmp(buffer, "AUTH_OK") == 0) {
        printf("===== LOGIN RIUSCITO =====\n");
        printf("Ho effettuato il login\n");
        show_main_menu(sock, username);
    } else if (strcmp(buffer, "AUTH_FAIL") == 0) {
        printf("===== ERRORE =====\n");
        printf("Username o password non validi!\n");
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

void show_main_menu(int sock, char *username) {
    int choice = 0;
    // Loop principale del menu
    while (1) {
        clear_screen();
        printf("===== MENU PRINCIPALE =====\n");
        printf("1. Mostra catalogo\n");
        printf("2. Cerca Film\n");
        printf("3. Mostra carrello\n");
        printf("4. Mostra notifiche\n");
        printf("5. Logout\n");
        printf("Scelta: ");
        scanf("%d", &choice);
        getchar(); // Consuma il carattere newline
        
        switch (choice) {
            case 1:
                view_catalogo(sock);
                break;
            case 2:
                printf("Funzione non implementata\n");
                printf("\nPremi invio per continuare...");
                getchar();
                break;
            case 3:
                printf("Funzione non implementata\n");
                printf("\nPremi invio per continuare...");
                getchar();
                break;
            case 4:
                printf("Funzione non implementata\n");
                printf("\nPremi invio per continuare...");
                getchar();
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

// Funzione per visualizzare il catalogo
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
        display_films(buffer);
    }
    
    printf("\nPremi invio per tornare al menu principale...");
    getchar(); // Attende che l'utente prema invio
}

void display_films(const char *film_data) {
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
    
    char titolo[100] = "";
    char copie_disponibili[10] = "";
    char genere[50] = "";
    int film_index = 0;
    
    while ((token = strtok_r(rest, "\n", &rest)) != NULL) {
        if (strcmp(token, "---") == 0) {
            // Stampa il film corrente
            if (titolo[0] != '\0') {
                printf("Film #%d\n", ++film_index);
                printf("\tTitolo: %s\n", titolo);
                printf("\tGenere: %s\n", genere);
                printf("\tCopie Disponibili: %s\n\n", copie_disponibili);
                
                // Resetta i campi per il prossimo film
                titolo[0] = '\0';
                genere[0] = '\0';
                copie_disponibili[0] = '\0';
            }
            continue;
        }
        
        char *key = token;
        char *value = strchr(token, ':');
        
        if (value != NULL) {
            *value = '\0';  // Termina la chiave
            value += 2;     // Salta il ':' e lo spazio
            
            if (strcasecmp(key, "titolo") == 0) {
                strncpy(titolo, value, sizeof(titolo) - 1);
                titolo[sizeof(titolo) - 1] = '\0';
            } else if (strcasecmp(key, "genere") == 0) {
                strncpy(genere, value, sizeof(genere) - 1);
                genere[sizeof(genere) - 1] = '\0';
            } else if (strcasecmp(key, "copie_disponibili") == 0) {
                strncpy(copie_disponibili, value, sizeof(copie_disponibili) - 1);
                copie_disponibili[sizeof(copie_disponibili) - 1] = '\0';
            }
        }
    }
    
    // Stampa l'ultimo film se c'è
    if (titolo[0] != '\0') {
        printf("Film #%d\n", ++film_index);
        printf("\tTitolo: %s\n", titolo);
        printf("\tGenere: %s\n", genere);
        printf("\tCopie Disponibili: %s\n", copie_disponibili);
    }
    
    free(data);
}