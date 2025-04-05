#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libpq-fe.h>
#include <pthread.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define MAX_FILMS 20

extern pthread_mutex_t db_mutex;

// Funzione per gestire le richieste di notifiche
void handle_notification_request(int client_socket, PGconn *conn, char *id_utente_input);

// Funzione per verificare le notifiche nel database
int check_notifications(PGconn *conn, const char *id_utente);

#endif