#include "auth.h"

// Funzione per gestire le richieste di login
void handle_login_request(int client_socket, PGconn *conn, char *credentials) {
    char *username = strtok(credentials, ":");
    char *password = strtok(NULL, ":");
    
    if (username != NULL && password != NULL) {
        printf("Tentativo di login per l'utente: %s\n", username);
        
        // Verifica le credenziali nel database
        if (verify_credentials(conn, username, password)) {
            send(client_socket, "AUTH_OK", 7, 0);
            printf("Login riuscito per l'utente: %s\n", username);
        } else {
            send(client_socket, "AUTH_FAIL", 9, 0);
            printf("Login fallito per l'utente: %s\n", username);
        }
    } else {
        send(client_socket, "FORMATO_ERRATO", 14, 0);
    }
}

// Funzione per verificare le credenziali nel database
int verify_credentials(PGconn *conn, const char *username, const char *password) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT * FROM utenti WHERE username='%s' AND password='%s'", 
             username, password);
    
    PGresult *result = PQexec(conn, query);
    
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query fallita: %s\n", PQerrorMessage(conn));
        PQclear(result);
        return 0;
    }
    
    // Verifica se è stato trovato un record
    int found = (PQntuples(result) > 0);
    
    PQclear(result);
    
    return found;
}