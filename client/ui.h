#ifndef UI_H
#define UI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

// Funzione per pulire lo schermo
void clear_screen();

// Funzione per mostrare il menu principale dopo il login
void show_main_menu(int sock, char *username, int* num_film, int* id_utente_loggato, int* carrello);

// Funzione per ricevere il catalogo
void view_catalogo(int sock);

// Funzione per visualizzare i film in un formato leggibile
void display_films(const char *film_data, int options);

// Funzione per mostrare il menu di ricerca film
void show_search_menu(int sock, int* num_film, int* carrello);

// Funzione per mostrare il carrello
void show_cart(int sock, int* num_film, int* id_utente_loggato, int* carrello);

// Funzione per mostrare le notifiche
void show_notifications(int sock, int* id_utente_loggato);

// Funzione per cercare film per titolo
void search_by_title(int sock);

// Funzione per cercare film per genere
void search_by_genre(int sock);

// Funzione per cercare film più popolari
void search_by_popularity(int sock);

// Funzione per visualizzare i film noleggiati
void retrieve_loaned_films(int sock, int* id_utente_loggato);

// Funzione per mostrare le notifiche
void show_notifications(int sock, int* id_utente_loggato);

// Funzione per aggiungere un film al carrello
void add_to_cart(int sock, int* num_film, int* carrello);

// Funzione per visualizzare il logo CineThreads (definita in client.c)
void display_logo();

#endif