#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "auth.h"
#include "ui.h"

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int id_utente_loggato = 0; // ID dell'utente loggato
int carrello[5] = {0}; // Array per memorizzare gli ID dei film nel carrello
int num_film = 0; // Numero di film nel carrello

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
        if (scanf("%d", &choice) != 1) {
            // Input non valido (non è un numero)
            while (getchar() != '\n'); // Pulisci il buffer di input
            choice = 0; // Imposta una scelta non valida per entrare nel default
        } else {
            getchar(); // Consuma il carattere newline solo se l'input era valido
        }
        
        switch (choice) {
            case 1:
                do_login(sock, &num_film, &id_utente_loggato, carrello);
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
                printf("\nPremi invio per continuare...");
                getchar();
                clear_screen();
        }
    }
    
    return 0;
}