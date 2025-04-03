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

#endif