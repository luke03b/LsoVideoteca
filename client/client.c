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

int id_utente_loggato = 0; // ID dell'utente loggato
int carrello[5] = {0}; // Array per memorizzare gli ID dei film nel carrello
int num_film = 0; // Numero di film nel carrello

// Funzione per visualizzare il logo CineThreads
void display_logo() {
    printf("\n");
    printf(COLOR_CYAN BOLD);
    printf("   ██████╗██╗███╗   ██╗███████╗████████╗██╗  ██╗██████╗ ███████╗ █████╗ ██████╗ ███████╗\n");
    printf("  ██╔════╝██║████╗  ██║██╔════╝╚══██╔══╝██║  ██║██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔════╝\n");
    printf("  ██║     ██║██╔██╗ ██║█████╗     ██║   ███████║██████╔╝█████╗  ███████║██║  ██║███████╗\n");
    printf("  ██║     ██║██║╚██╗██║██╔══╝     ██║   ██╔══██║██╔══██╗██╔══╝  ██╔══██║██║  ██║╚════██║\n");
    printf("  ╚██████╗██║██║ ╚████║███████╗   ██║   ██║  ██║██║  ██║███████╗██║  ██║██████╔╝███████║\n");
    printf("   ╚═════╝╚═╝╚═╝  ╚═══╝╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═════╝ ╚══════╝\n");
    printf(COLOR_RESET);
    printf(COLOR_YELLOW "                  🎬 Il tuo cinema digitale preferito 🎬\n" COLOR_RESET);
    printf("\n");
}

int main() {
    clear_screen();
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    
    // Creazione socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf(COLOR_RED "\n[✘] Errore nella creazione del socket\n" COLOR_RESET);
        return -1;
    }
    
    // Configurazione dell'indirizzo del server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Conversione indirizzo IP da testo a binario
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        printf(COLOR_RED "\n[✘] Indirizzo IP non valido / non supportato\n" COLOR_RESET);
        return -1;
    }
    
    // Tentativo di connessione al server
    printf(COLOR_CYAN "[⟳] Tentativo di connessione al server %s:%d...\n" COLOR_RESET, SERVER_IP, PORT);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf(COLOR_RED "\n[✘] Connessione fallita\n" COLOR_RESET);
        return -1;
    }
    
    // Leggi messaggio di benvenuto dal server
    read(sock, buffer, BUFFER_SIZE);
    printf(COLOR_GREEN "[✔] Server: %s\n\n" COLOR_RESET, buffer);
    
    int choice = 0;
    
    // Loop principale del menu
    while (1) {
        display_logo();
        
        printf("\n");
        printf(BOLD COLOR_CYAN "╔════════════════════════════════╗\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "║" COLOR_WHITE "      MENU AUTENTICAZIONE       " COLOR_CYAN "║\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "╠════════════════════════════════╣\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "║" COLOR_RESET " 🔑 " COLOR_WHITE "1. Login                    " COLOR_CYAN "║\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "║" COLOR_RESET " 📝 " COLOR_WHITE "2. Registrazione            " COLOR_CYAN "║\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "║" COLOR_RESET " 🚪 " COLOR_WHITE "3. Esci                     " COLOR_CYAN "║\n" COLOR_RESET);
        printf(BOLD COLOR_CYAN "╚════════════════════════════════╝\n" COLOR_RESET);
        printf("\n");
        printf(COLOR_YELLOW "Scelta: " COLOR_RESET);
        
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
                clear_screen();
                display_logo();
                printf(COLOR_GREEN "\n[✔] Disconnessione dal server. Arrivederci!\n" COLOR_RESET);
                return 0;
            default:
                printf(COLOR_RED "\n[✘] Scelta non valida. Riprova.\n" COLOR_RESET);
                printf("\nPremi invio per continuare...");
                getchar();
                clear_screen();
        }
    }
    
    return 0;
}