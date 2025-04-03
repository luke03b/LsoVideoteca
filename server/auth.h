#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libpq-fe.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

// Funzione per gestire le richieste di login
void handle_login_request(int client_socket, PGconn *conn, char *credentials);

// Funzione per verificare le credenziali nel database
int verify_credentials(PGconn *conn, const char *username, const char *password);

#endif