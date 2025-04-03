#ifndef FILM_H
#define FILM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libpq-fe.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

// Funzione per gestire le richieste di visualizzazione catalogo
void handle_catalogo_request(int client_socket, PGconn *conn);

// Funzione per inviare film al cliente
int recupera_film(PGconn *conn, int client_socket);

#endif