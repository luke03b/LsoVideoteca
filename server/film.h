#ifndef FILM_H
#define FILM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libpq-fe.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define MAX_FILMS 20

// Enumerazione per i tipi di ricerca
typedef enum {
    SEARCH_TITLE = 1,
    SEARCH_GENRE = 2,
    SEARCH_POPULARITY = 3
} SearchType;

// Struttura per rappresentare i dati di un film
typedef struct {
    char titolo[256];
    char genere[101];
    int copie_disponibili;
} Film;

// Funzione per gestire le richieste di catalogo film
void handle_catalogo_request(int client_socket, PGconn *conn);

// Funzione per gestire le richieste di ricerca film
void handle_search_request(int client_socket, PGconn *conn, char *search_params);

// Funzione per ottenere i film dal database
int get_films(PGconn *conn, Film *films, int max_films);

// Funzione per cercare film nel database
int search_films(PGconn *conn, Film *films, int max_films, SearchType type, const char *query);

// Funzione per formattare i dati dei film per l'invio al client
void format_films_data(Film *films, int num_films, char *output, size_t output_size);

#endif