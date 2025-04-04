#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

// Funzione per pulire lo schermo
void clear_screen();

// Funzione per mostrare le notifiche
void show_notifications(int sock, int* id_utente_loggato);

#endif