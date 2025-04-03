#include "film.h"

// Funzione per gestire le richieste di catalogo film
void handle_catalogo_request(int client_socket, PGconn *conn) {
    Film films[MAX_FILMS];
    char response[BUFFER_SIZE * 20] = {0};
    int num_films;
    
    printf("Richiesta di catalogo film ricevuta\n");
    
    // Ottieni i film dal database
    num_films = get_films(conn, films, MAX_FILMS);
    
    if (num_films <= 0) {
        // Nessun film trovato o errore di database
        send(client_socket, "FILM_FAIL", 9, 0);
        printf("Errore nel recupero dei film dal database\n");
        return;
    }
    
    // Formatta i dati dei film per l'invio al client
    format_films_data(films, num_films, response, sizeof(response));
    
    // Invia i dati al client
    send(client_socket, response, strlen(response), 0);
    printf("Inviati dati di %d film al client\n", num_films);
}

// Funzione per gestire le richieste di ricerca film
void handle_search_request(int client_socket, PGconn *conn, char *search_params) {
    Film films[MAX_FILMS];
    char response[BUFFER_SIZE * 20] = {0};
    int num_films = 0;
    
    // Estrai tipo di ricerca e query dai parametri
    char *search_type_str = strtok(search_params, ":");
    char *search_query = strtok(NULL, ":");
    
    if (search_type_str == NULL || search_query == NULL) {
        send(client_socket, "SEARCH_FAIL", 11, 0);
        printf("Parametri di ricerca non validi\n");
        return;
    }
    
    int search_type = atoi(search_type_str);
    printf("Richiesta di ricerca film ricevuta. Tipo: %d, Query: %s\n", search_type, search_query);
    
    // Esegui la ricerca in base al tipo
    num_films = search_films(conn, films, MAX_FILMS, search_type, search_query);
    
    if (num_films <= 0) {
        // Nessun film trovato o errore di database
        send(client_socket, "SEARCH_FAIL", 11, 0);
        printf("Nessun film trovato o errore nella ricerca\n");
        return;
    }
    
    // Formatta i dati dei film per l'invio al client
    format_films_data(films, num_films, response, sizeof(response));
    
    // Invia i dati al client
    send(client_socket, response, strlen(response), 0);
    printf("Inviati dati di %d film trovati al client\n", num_films);
}

// Funzione per ottenere i film dal database
int get_films(PGconn *conn, Film *films, int max_films) {
    char query[BUFFER_SIZE];
    PGresult *result;
    int num_films = 0;
    
    // Crea la query per ottenere i film
    snprintf(query, BUFFER_SIZE, 
             "SELECT id, titolo, genere, copie_disponibili FROM film ORDER BY titolo LIMIT %d", 
             max_films);
    
    // Esegui la query
    result = PQexec(conn, query);
    
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query fallita: %s\n", PQerrorMessage(conn));
        PQclear(result);
        return 0;
    }
    
    // Ottieni il numero di film recuperati
    num_films = PQntuples(result);
    
    // Riempi l'array di film
    for (int i = 0; i < num_films; i++) {
        // Aggiungi l'ID del film
        strncpy(films[i].id, PQgetvalue(result, i, 0), sizeof(films[i].id) - 1);
        films[i].id[sizeof(films[i].id) - 1] = '\0';
        
        strncpy(films[i].titolo, PQgetvalue(result, i, 1), sizeof(films[i].titolo) - 1);
        films[i].titolo[sizeof(films[i].titolo) - 1] = '\0';
        
        strncpy(films[i].genere, PQgetvalue(result, i, 2), sizeof(films[i].genere) - 1);
        films[i].genere[sizeof(films[i].genere) - 1] = '\0';
        
        films[i].copie_disponibili = atoi(PQgetvalue(result, i, 3));
    }
    
    PQclear(result);
    return num_films;
}

// Funzione per cercare film nel database
int search_films(PGconn *conn, Film *films, int max_films, SearchType type, const char *query) {
    char sql_query[BUFFER_SIZE];
    PGresult *result;
    int num_films = 0;
    
    // Crea la query SQL in base al tipo di ricerca
    switch (type) {
        case SEARCH_TITLE:
            // Ricerca per titolo (case insensitive)
            snprintf(sql_query, BUFFER_SIZE, 
                    "SELECT id, titolo, genere, copie_disponibili FROM film "
                    "WHERE LOWER(titolo) LIKE LOWER('%%%s%%') "
                    "ORDER BY titolo LIMIT %d", 
                    query, max_films);
            break;
            
        case SEARCH_GENRE:
            // Ricerca per genere (case insensitive)
            snprintf(sql_query, BUFFER_SIZE, 
                    "SELECT id, titolo, genere, copie_disponibili FROM film "
                    "WHERE LOWER(genere) LIKE LOWER('%%%s%%') "
                    "ORDER BY titolo LIMIT %d", 
                    query, max_films);
            break;
            
        case SEARCH_POPULARITY:
            // Ricerca per popolarità (film più noleggiati)
            snprintf(sql_query, BUFFER_SIZE, 
                    "SELECT id, titolo, genere, copie_disponibili FROM film "
                    "ORDER BY noleggi_totali DESC LIMIT %d", 
                    max_films);
            break;
            
        default:
            fprintf(stderr, "Tipo di ricerca non valido: %d\n", type);
            return 0;
    }
    
    // Esegui la query
    result = PQexec(conn, sql_query);
    
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query di ricerca fallita: %s\n", PQerrorMessage(conn));
        PQclear(result);
        return 0;
    }
    
    // Ottieni il numero di film recuperati
    num_films = PQntuples(result);
    
    // Riempi l'array di film
    for (int i = 0; i < num_films; i++) {
        // Aggiungi l'ID del film
        strncpy(films[i].id, PQgetvalue(result, i, 0), sizeof(films[i].id) - 1);
        films[i].id[sizeof(films[i].id) - 1] = '\0';
        
        strncpy(films[i].titolo, PQgetvalue(result, i, 1), sizeof(films[i].titolo) - 1);
        films[i].titolo[sizeof(films[i].titolo) - 1] = '\0';
        
        strncpy(films[i].genere, PQgetvalue(result, i, 2), sizeof(films[i].genere) - 1);
        films[i].genere[sizeof(films[i].genere) - 1] = '\0';
        
        films[i].copie_disponibili = atoi(PQgetvalue(result, i, 3));
    }
    
    PQclear(result);
    return num_films;
}

// Funzione per formattare i dati dei film per l'invio al client
void format_films_data(Film *films, int num_films, char *output, size_t output_size) {
    char *ptr = output;
    size_t remaining = output_size;
    int chars_written;
    
    // Scrivi il numero di film
    chars_written = snprintf(ptr, remaining, "%d\n", num_films);
    ptr += chars_written;
    remaining -= chars_written;
    
    // Scrivi i dati di ogni film
    for (int i = 0; i < num_films; i++) {
        // ID
        chars_written = snprintf(ptr, remaining, "id: %s\n", films[i].id);
        ptr += chars_written;
        remaining -= chars_written;
        
        // Titolo
        chars_written = snprintf(ptr, remaining, "titolo: %s\n", films[i].titolo);
        ptr += chars_written;
        remaining -= chars_written;
        
        // Genere
        chars_written = snprintf(ptr, remaining, "genere: %s\n", films[i].genere);
        ptr += chars_written;
        remaining -= chars_written;
        
        // Copie disponibili
        chars_written = snprintf(ptr, remaining, "copie_disponibili: %d\n", films[i].copie_disponibili);
        ptr += chars_written;
        remaining -= chars_written;
        
        // Separatore tra film
        chars_written = snprintf(ptr, remaining, "---\n");
        ptr += chars_written;
        remaining -= chars_written;
    }
}