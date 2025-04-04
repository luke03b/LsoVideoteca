#include "notification.h"

// Funzione per gestire le richieste di login
void handle_notification_request(int client_socket, PGconn *conn, char *id_utente_input) {
    char *id_utente = strtok(id_utente_input, ":");
    
    if (id_utente != NULL) {
        printf("Recupero notifiche per l'utente: %s\n", id_utente);
        
        // Verifica le credenziali e ottieni l'ID dell'utente
        int result = check_notifications(conn, id_utente);
        
        if (result > 0) {
            char response[20];
            snprintf(response, sizeof(response), "NOTIFICATION_OK");
            send(client_socket, response, strlen(response), 0);
            printf("Notifiche recuperate per l'utente con id: %s\n", id_utente);
        } else {
            send(client_socket, "NOTIFICATION_FAIL", 17, 0);
            printf("Notifiche fallite per l'utente con id: %s\n", id_utente);
        }
    } else {
        send(client_socket, "FORMATO_ERRATO", 14, 0);
    }
}


// Funzione per verificare le credenziali nel database
int check_notifications(PGconn *conn, const char *id_utente) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, 
                "SELECT * "
                "FROM noleggi "
                "WHERE id_utente = '%s' "
                "AND restituito = FALSE "
                "AND data_restituzione < CURRENT_DATE;", 
            id_utente);
    
    PGresult *result = PQexec(conn, query);
    
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query fallita: %s\n", PQerrorMessage(conn));
        PQclear(result);
        return -1;  // Restituisci -1 in caso di errore
    }
    
    
    int return_value;
    // Verifica se è stato trovato un record
    if (PQntuples(result) > 0) {
        return_value = 1;
    } else {
        return_value = 0;
    }
    
    
    PQclear(result);
    
    return return_value;  // Restituisce -1 se non trovato, altrimenti l'ID utente
}