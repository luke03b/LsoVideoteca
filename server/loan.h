#ifndef LOAN_H
#define LOAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libpq-fe.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define MAX_FILMS 20

// Gestione richiesta noleggio
void handle_loan_request(int client_socket, PGconn *conn, char *info);

// Funzione per effettuare il noleggio
int do_loan(PGconn *conn, const char *id_utente, const char *id_film);

#endif