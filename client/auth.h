#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

// Funzione per pulire lo schermo
void clear_screen();

// Funzione per effettuare il login
void do_login(int sock, int* num_film, int* id_utente_loggato, int* carrello);

// Funzione per effettuare la registrazione
void do_registration(int sock);

// Funzione per mostrare il menu principale dopo il login
void show_main_menu(int sock, char *username, int* num_film, int* id_utente_loggato, int* carrello);

#endif