#ifndef LOAN_H
#define LOAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

// Funzione per pulire lo schermo
void clear_screen();

// Funzione per aggiungere un film al carrello
void add_to_cart(int sock, int* num_film, int* carrello);

// Funzione per mostrare il carrello
void show_cart(int sock, int* num_film, int* id_utente_loggato, int* carrello);

// Funzione per effettuare il check-out
void check_out(int sock, int* num_film, int* id_utente_loggato, int* carrello);

// Funzione per controllare se un film è già nel carrello
int is_film_already_in_cart(int id_film, int* num_film, int* carrello);

// Funzione per visualizzare i film noleggiati
void retrieve_loaned_films(int sock, int* id_utente_loggato);

// Funzione per restituire un film
void restituisci_film(int sock, int* id_utente_loggato);

// Funzione per visualizzare i film in un formato leggibile
void display_films(const char *film_data, int options);

#endif