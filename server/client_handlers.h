#ifndef CLIENT_HANDLERS_H
#define CLIENT_HANDLERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <libpq-fe.h>

#define BUFFER_SIZE 1024

// Struttura per passare dati al thread
typedef struct {
    int socket;
    PGconn *conn;
} thread_args;

// Funzione per gestire la connessione con un client
void *handle_client(void *arg);

// Funzione per gestire le richieste di login (implementata in auth.c)
void handle_login_request(int client_socket, PGconn *conn, char *credentials);

// Funzione per gestire le richieste di registrazione (implementata in auth.c)
void handle_registration_request(int client_socket, PGconn *conn, char *credentials);

// Funzione per gestire le richieste di visualizzazione catalogo (implementata in film.c)
void handle_catalogo_request(int client_socket, PGconn *conn);

// Funzione per gestire le richieste di ricerca film (implementata in film.c)
void handle_search_request(int client_socket, PGconn *conn, char *search_params);

// Funzione per gestire le richieste di noleggio film (implementata in loan.c)
void handle_loan_request(int client_socket, PGconn *conn, char *info);

// Funzione per gestire le richieste di restituzione film (implementata in loan.c)
void handle_return_request(int client_socket, PGconn *conn, char *info);

#endif