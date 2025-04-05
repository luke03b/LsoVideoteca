#include "loan.h"

void handle_loan_request(int client_socket, PGconn *conn, char *info) {
    char *id_utente = strtok(info, ":");
    char *id_film = strtok(NULL, ":");
    
    if (id_utente != NULL && id_film != NULL) {
        printf("Tentativo di noleggio per l'utente: %s, film: %s\n", id_utente, id_film);
        
        // Verifica le credenziali nel database
        if (do_loan(conn, id_utente, id_film)) {
            send(client_socket, "LOAN_OK", 7, 0);
            printf("Noleggio riuscito per l'utente: %s, film: %s\n", id_utente, id_film);
        } else {
            send(client_socket, "LOAN_FAIL", 9, 0);
            printf("Noleggio fallito per l'utente: %s, film: %s\n", id_utente, id_film);
        }
    } else {
        send(client_socket, "FORMATO_ERRATO", 14, 0);
    }
}

// Funzione per verificare le credenziali nel database
int do_loan(PGconn *conn, const char *id_utente, const char *id_film) {
    pthread_mutex_lock(&db_mutex);
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT noleggia_film('%s', '%s')", id_utente, id_film);
    
    PGresult *result = PQexec(conn, query);

    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query fallita: %s\n", PQerrorMessage(conn));
        PQclear(result);
        return 0;
    }

    // Controlla se il risultato è TRUE (t) o FALSE (f)
    char *success_value = PQgetvalue(result, 0, 0);
    int success = (strcmp(success_value, "t") == 0);

    PQclear(result);

    if (success) {
        printf("Noleggio completato con successo!\n");
        // Altre operazioni in caso di successo
    } else {
        printf("Impossibile noleggiare il film. Nessuna copia disponibile o l'utente ha già noleggiato questo film.\n");
        // Altre operazioni in caso di fallimento
    }

    pthread_mutex_unlock(&db_mutex);
    return success;
}
void handle_return_request(int client_socket, PGconn *conn, char *info) {
    char *id_utente = strtok(info, ":");
    char *id_film = strtok(NULL, ":");
    
    if (id_utente != NULL && id_film != NULL) {
        printf("Tentativo di restituzione per l'utente: %s, film: %s\n", id_utente, id_film);
        
        // Verifica le credenziali nel database
        if (return_loan(conn, id_utente, id_film)) {
            send(client_socket, "RETURN_OK", 9, 0);
            printf("Restituzione riuscita per l'utente: %s, film: %s\n", id_utente, id_film);
        } else {
            send(client_socket, "RETURN_FAIL", 11, 0);
            printf("Restituzione fallita per l'utente: %s, film: %s\n", id_utente, id_film);
        }
    } else {
        send(client_socket, "FORMATO_ERRATO", 14, 0);
    }
}

// Funzione per verificare le credenziali nel database
int return_loan(PGconn *conn, const char *id_utente, const char *id_film) {
    pthread_mutex_lock(&db_mutex);
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT restituisci_film('%s', '%s')", id_utente, id_film);
    
    PGresult *result = PQexec(conn, query);

    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query fallita: %s\n", PQerrorMessage(conn));
        PQclear(result);
        return 0;
    }

    // Controlla se il risultato è TRUE (t) o FALSE (f)
    char *success_value = PQgetvalue(result, 0, 0);
    int success = (strcmp(success_value, "t") == 0);

    PQclear(result);

    if (success) {
        printf("Restituzione completata con successo!\n");
        // Altre operazioni in caso di successo
    } else {
        printf("Restituzione fallita. L'utente non ha un noleggio attivo per questo film\n");
        // Altre operazioni in caso di fallimento
    }

    pthread_mutex_unlock(&db_mutex);
    return success;
}