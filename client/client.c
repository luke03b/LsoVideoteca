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
                printf("Funzione non implementata\n");
                printf("\nPremi invio per continuare...");
                getchar();
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