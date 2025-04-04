#ifndef FILM_H
#define FILM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

// Funzione per pulire lo schermo
void clear_screen();

// Funzione per cercare film per titolo
void search_by_title(int sock);

// Funzione per cercare film per genere
void search_by_genre(int sock);

// Funzione per cercare film più popolari
void search_by_popularity(int sock);

// Funzione per visualizzare i film in un formato leggibile
void display_films(const char *film_data, int options);

#endif