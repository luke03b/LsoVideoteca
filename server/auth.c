#include "auth.h"

// Funzione per gestire le richieste di login
void handle_login_request(int client_socket, PGconn *conn, char *credentials) {
    char *username = strtok(credentials, ":");
    char *password = strtok(NULL, ":");
    
    if (username != NULL && password != NULL) {
        printf("Tentativo di login per l'utente: %s\n", username);
        
        // Verifica le credenziali e ottieni l'ID dell'utente
        int user_id = verify_credentials(conn, username, password);
        
        if (user_id > 0) {
            // Aggiungi l'ID utente alla risposta di autenticazione riuscita
            char response[20];
            snprintf(response, sizeof(response), "AUTH_OK:%d", user_id);
            send(client_socket, response, strlen(response), 0);
            printf("Login riuscito per l'utente: %s (ID: %d)\n", username, user_id);
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
    pthread_mutex_lock(&db_mutex);
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT id FROM utenti WHERE username='%s' AND password='%s'", 
             username, password);
    
    PGresult *result = PQexec(conn, query);
    
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query fallita: %s\n", PQerrorMessage(conn));
        PQclear(result);
        return -1;  // Restituisci -1 in caso di errore
    }
    
    int user_id = -1;
    
    // Verifica se è stato trovato un record
    if (PQntuples(result) > 0) {
        // Estrai l'ID dalla prima riga
        user_id = atoi(PQgetvalue(result, 0, 0));
    }
    
    PQclear(result);
    pthread_mutex_unlock(&db_mutex);
    return user_id;  // Restituisce -1 se non trovato, altrimenti l'ID utente
}



// Funzione per gestire le richieste di registrazione
void handle_registration_request(int client_socket, PGconn *conn, char *credentials) {
    char *username = strtok(credentials, ":");
    char *password = strtok(NULL, ":");
    
    if (username != NULL && password != NULL) {
        printf("Tentativo di registrazione per l'utente: %s\n", username);
        
        // Verifica le credenziali nel database
        if (create_user(conn, username, password)) {
            send(client_socket, "REGISTRATION_OK", 15, 0);
            printf("Registrazione riuscita per l'utente: %s\n", username);
        } else {
            send(client_socket, "REGISTRATION_FAIL", 17, 0);
            printf("Registrazione fallita per l'utente: %s\n", username);
        }
    } else {
        send(client_socket, "FORMATO_ERRATO", 14, 0);
    }
}

// Funzione per creare un nuovo utente nel database
int create_user(PGconn *conn, const char *username, const char *password) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "INSERT INTO utenti (username, password) VALUES ('%s', '%s')", 
             username, password);
    
    PGresult *result = PQexec(conn, query);
    
    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Query fallita: %s\n", PQerrorMessage(conn));
        PQclear(result);
        return 0;
    }
    
    // Verifica quante righe sono state inserite
    int rows_affected = atoi(PQcmdTuples(result));
    
    PQclear(result);
    
    return rows_affected > 0;
}