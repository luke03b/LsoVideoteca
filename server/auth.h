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

// Funzione per gestire le richieste di registrazione
void handle_registration_request(int client_socket, PGconn *conn, char *credentials);

// Funzione per creare un nuovo utente nel database
int create_user(PGconn *conn, const char *username, const char *password);

#endif