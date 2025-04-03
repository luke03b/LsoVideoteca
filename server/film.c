#include "film.h"

void handle_catalogo_request(int client_socket, PGconn *conn) {
    
    printf("Tentativo di recupero catalogo film\n");
    
    // Recupera film dal database
    if (recupera_film(conn, client_socket)) {
        printf("Film recuperati e inviati al client\n");
    } else {
        send(client_socket, "FILM_FAIL", 9, 0);
        printf("Film non recuperati\n");
    }
}

int recupera_film(PGconn *conn, int client_socket) {
    char query[BUFFER_SIZE];
    snprintf(query, BUFFER_SIZE, "SELECT titolo, genere, copie_disponibili FROM film");
    
    PGresult *result = PQexec(conn, query);
    
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query fallita: %s\n", PQerrorMessage(conn));
        PQclear(result);
        return 0;
    }
    
    // Verifica se è stato trovato un record
    int nrows = PQntuples(result);
    if (nrows == 0) {
        return 0;
    }

    // Ottieni il numero di colonne
    int ncols = PQnfields(result);
    
    // Prepara un buffer per contenere tutti i dati da inviare
    char response[BUFFER_SIZE * 10] = ""; // Buffer più grande per contenere più film
    char temp[BUFFER_SIZE] = "";
    
    // Aggiungi il numero di film al messaggio
    sprintf(temp, "%d\n", nrows);
    strcat(response, temp);
    
    // Per ogni riga (film)
    for (int i = 0; i < nrows; i++) {
        // Per ogni colonna
        for (int j = 0; j < ncols; j++) {
            // Ottieni il nome della colonna
            const char* colName = PQfname(result, j);
            // Ottieni il valore
            const char* value = PQgetvalue(result, i, j);
            
            // Aggiungi al buffer temporaneo
            sprintf(temp, "%s: %s\n", colName, value);
            
            // Controlla se c'è spazio nel buffer di risposta
            if (strlen(response) + strlen(temp) < sizeof(response) - 1) {
                strcat(response, temp);
            } else {
                fprintf(stderr, "Buffer di risposta pieno. Troncamento dei dati.\n");
                break;
            }
        }
        // Aggiungi un separatore tra i film
        if (i < nrows - 1) {
            strcat(response, "---\n");
        }
    }

    // Invia i dati al client
    send(client_socket, response, strlen(response), 0);
    
    PQclear(result);
    
    return 1; // Successo
}