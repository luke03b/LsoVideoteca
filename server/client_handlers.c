#include "client_handlers.h"

// Funzione per gestire la connessione con un client
void *handle_client(void *arg) {
    thread_args *args = (thread_args *)arg;
    int client_socket = args->socket;
    PGconn *conn = args->conn;
    char buffer[BUFFER_SIZE] = {0};
    int read_size;
    
    // Invia un messaggio di conferma di connessione al client
    char *welcome_message = "CONNESSO";
    send(client_socket, welcome_message, strlen(welcome_message), 0);
    
    // Loop per gestire le richieste del client
    while ((read_size = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0'; // Aggiungi terminatore di stringa
        
        // Controllo se il messaggio è una richiesta di login
        if (strncmp(buffer, "LOGIN:", 6) == 0) {
            handle_login_request(client_socket, conn, buffer + 6);
        } else if (strncmp(buffer, "REGISTRAZIONE:", 14) == 0) {
            handle_registration_request(client_socket, conn, buffer + 14);
        } else if (strcmp(buffer, "CATALOGO") == 0) {
            handle_catalogo_request(client_socket, conn);
        } else if (strncmp(buffer, "SEARCH:", 7) == 0) {
            handle_search_request(client_socket, conn, buffer + 7);
        } else if (strncmp(buffer, "NOLEGGIO:", 9) == 0){
            handle_loan_request(client_socket, conn, buffer + 9);
        } else {
            // Gestisci altri comandi se necessario
            send(client_socket, "COMANDO_SCONOSCIUTO", 19, 0);
        }
        
        // Pulisci il buffer
        memset(buffer, 0, BUFFER_SIZE);
    }
    
    if (read_size == 0) {
        printf("Client disconnesso\n");
    } else if (read_size == -1) {
        perror("Errore di ricezione");
    }
    
    close(client_socket);
    free(args);
    
    return NULL;
}