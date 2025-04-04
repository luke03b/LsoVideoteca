#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

// Funzione per effettuare il login
void do_login(int sock);

// Funzione per effettuare la registrazione
void do_registration(int sock);

// Funzione per mostrare il menu principale dopo il login
void show_main_menu(int sock, char *username);

// Funzione per ricevere il catalogo
void view_catalogo(int sock);

// Funzione per visualizzare i film in un formato leggibile
void display_films(const char *film_data, int options);

// Funzione per mostrare il menu di ricerca film
void show_search_menu(int sock);

// Funzione per cercare film per titolo
void search_by_title(int sock);

// Funzione per cercare film per genere
void search_by_genre(int sock);

// Funzione per cercare film più popolari
void search_by_popularity(int sock);

// Funzione per aggiungere un film al carrello
void add_to_cart(int sock);

// Funzione per mostrare il carrello
void show_cart(int sock);

// Funzione per controllare se un film è già nel carrello
int is_film_already_in_cart(int id_film);

// Funzione per effettuare il check-out
void check_out(int sock);

// Funzione per visualizzare i film noleggiati
void retrieve_loaned_films(int sock);

// Funzione per restituire un film
void restituisci_film(int sock);

// Funzione per mostrare le notifiche
void show_notifications(int sock);

// Funzione per pulire lo schermo
void clear_screen() {
    printf("\033[H\033[J");
}

int id_utente_loggato = 0; // ID dell'utente loggato
int carrello[5] = {0}; // Array per memorizzare gli ID dei film nel carrello
int num_film = 0; // Numero di film nel carrello

int main() {
    clear_screen();
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    
    // Creazione socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nErrore nella creazione del socket\n");
        return -1;
    }
    
    // Configurazione dell'indirizzo del server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Conversione indirizzo IP da testo a binario
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        printf("\nIndirizzo IP non valido / non supportato\n");
        return -1;
    }
    
    // Tentativo di connessione al server
    printf("Tentativo di connessione al server %s:%d...\n", SERVER_IP, PORT);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnessione fallita\n");
        return -1;
    }
    
    // Leggi messaggio di benvenuto dal server
    read(sock, buffer, BUFFER_SIZE);
    printf("Server: %s\n\n", buffer);
    
    int choice = 0;
    
    // Loop principale del menu
    while (1) {
        printf("===== MENU AUTENTICAZIONE =====\n");
        printf("1. Login\n");
        printf("2. Registrazione\n");
        printf("3. Esci\n");
        printf("Scelta: ");
        if (scanf("%d", &choice) != 1) {
            // Input non valido (non è un numero)
            while (getchar() != '\n'); // Pulisci il buffer di input
            choice = 0; // Imposta una scelta non valida per entrare nel default
        } else {
            getchar(); // Consuma il carattere newline solo se l'input era valido
        }
        
        switch (choice) {
            case 1:
                do_login(sock);
                break;
            case 2:
                do_registration(sock);
                break;
            case 3:
                close(sock);
                printf("Disconnessione dal server. Arrivederci!\n");
                return 0;
            default:
                printf("Scelta non valida. Riprova.\n");
                printf("\nPremi invio per continuare...");
                getchar();
                clear_screen();
        }
    }
    
    return 0;
}

// Funzione per effettuare il login
void do_login(int sock) {
    char username[100];
    char password[100];
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE] = {0};
    
    clear_screen();
    printf("===== PAGINA DI LOGIN =====\n");
    
    printf("Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Rimuovi il newline
    
    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Rimuovi il newline
    
    // Crea la richiesta di login
    snprintf(request, BUFFER_SIZE, "LOGIN:%s:%s", username, password);
    
    // Invia la richiesta al server
    send(sock, request, strlen(request), 0);
    
    // Ricevi la risposta
    read(sock, buffer, BUFFER_SIZE);
    
    clear_screen();
    
    // Controlla la risposta del server
    if (strncmp(buffer, "AUTH_OK:", 8) == 0) {
        // Estrai l'ID utente dalla risposta
        id_utente_loggato = atoi(buffer + 8);
        show_main_menu(sock, username);
    } else if (strcmp(buffer, "AUTH_FAIL") == 0) {
        id_utente_loggato = -1;  // Assicurati che l'ID sia -1 se il login fallisce
        printf("===== ERRORE =====\n");
        printf("Username o password non validi!\n");
        printf("\nPremi invio per continuare...");
        getchar();
        clear_screen();
    } else {
        id_utente_loggato = -1;  // Assicurati che l'ID sia -1 se c'è un errore
        printf("===== ERRORE =====\n");
        printf("Errore nella comunicazione con il server: %s\n", buffer);
        printf("\nPremi invio per continuare...");
        getchar();
        clear_screen();
    }
}

// Funzione per effettuare la registrazione
void do_registration(int sock) {
    char username[100];
    char password[100];
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE] = {0};
    
    clear_screen();
    printf("===== PAGINA DI REGISTRAZIONE =====\n");
    
    printf("Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Rimuovi il newline
    
    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Rimuovi il newline
    
    // Crea la richiesta di registrazione
    snprintf(request, BUFFER_SIZE, "REGISTRAZIONE:%s:%s", username, password);
    
    // Invia la richiesta al server
    send(sock, request, strlen(request), 0);
    
    // Ricevi la risposta
    read(sock, buffer, BUFFER_SIZE);
    
    clear_screen();
    
    // Controlla la risposta del server
    if (strcmp(buffer, "REGISTRATION_OK") == 0) {
        printf("===== REGISTRAZIONE RIUSCITA =====\n");
        printf("Ho effettuato la registrazione\n");
        printf("\nPremi invio per continuare...");
        getchar();
        clear_screen();
    } else if (strcmp(buffer, "REGISTRATION_FAIL") == 0) {
        printf("===== ERRORE =====\n");
        printf("Username o password non validi. Errore nella registrazione!\n");
        printf("\nPremi invio per continuare...");
        getchar();
        clear_screen();
    } else {
        printf("===== ERRORE =====\n");
        printf("Errore nella comunicazione con il server: %s\n", buffer);
        printf("\nPremi invio per continuare...");
        getchar();
        clear_screen();
    }
}

void show_main_menu(int sock, char *username) {
    int choice = 0;
    // Loop principale del menu
    while (1) {
        clear_screen();
        printf("===== BENVENUTO =====\n");
        printf("1. Noleggia Film\n");
        printf("2. Carrello\n");
        printf("3. Film noleggiati\n");
        printf("4. Notifiche\n");
        printf("5. Logout\n");
        printf("Scelta: ");
        if (scanf("%d", &choice) != 1) {
            // Input non valido (non è un numero)
            while (getchar() != '\n'); // Pulisci il buffer di input
            choice = 0; // Imposta una scelta non valida per entrare nel default
        } else {
            getchar(); // Consuma il carattere newline solo se l'input era valido
        }
        
        switch (choice) {
            case 1:
                show_search_menu(sock);
                break;
            case 2:
                show_cart(sock);
                break;
            case 3:
                retrieve_loaned_films(sock);
                break;
            case 4:
                show_notifications(sock);
                break;
            case 5:
                printf("Arrivederci\n");
                printf("\nPremi invio per continuare...");
                getchar();
                clear_screen();
                return;
            default:
                printf("Scelta non valida. Riprova.\n");
                printf("\nPremi invio per continuare...");
                getchar();
        }
    }
}

void show_notifications(int sock) {
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 20] = {0};
    int bytes_received;
    clear_screen();

    printf("===== NOTIFICHE =====\n");

    snprintf(request, BUFFER_SIZE, "NOTIFICHE:%d", id_utente_loggato);
    send(sock, request, strlen(request), 0);
    read(sock, buffer, BUFFER_SIZE);
    
    // Controlla la risposta del server
    if (strcmp(buffer, "NOTIFICATION_OK") == 0) {
        printf("ATTENZIONE: Hai dei film da restituire oltre la data consentita\n");
        printf("\nPremi invio per continuare...");
        getchar();
    } else if (strcmp(buffer, "NOTIFICATION_FAIL") == 0) {
        printf("Non hai nuove notifiche\n");
        printf("\nPremi invio per continuare...");
        getchar();
    } else {
        printf("===== ERRORE =====\n");
        printf("Errore nella comunicazione con il server: %s\n", buffer);
        printf("\nPremi invio per continuare...");
        getchar();
    }
}


// Funzione per visualizzare film in base a un array di ID
void show_cart(int sock) {
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 20] = {0};
    int bytes_received;

    while (1)
        {
        clear_screen();
        printf("===== CARRELLO =====\n\n");
        
        if (num_film <= 0) {
            printf("Nessun film nel carrello.\n");
            printf("\nPremi invio per tornare al menu principale...");
            getchar();
            return;
        }
        
        // Costruiamo una stringa con tutti gli ID separati da virgole
        char id_list[200] = {0};
        for (int i = 0; i < num_film; i++) {
            char id_str[10];
            snprintf(id_str, sizeof(id_str), "%d", carrello[i]);
            
            // Aggiungi virgola se non è il primo ID
            if (i > 0 && i < num_film) {
                strcat(id_list, ",");
            }
            strcat(id_list, id_str);
        }
        
        // Crea la richiesta di ricerca (3 = ricerca per ID multipli)
        snprintf(request, BUFFER_SIZE, "SEARCH:4:%s", id_list);
        send(sock, request, strlen(request), 0);
        
        bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received <= 0) {
            printf("Errore nella ricezione dei dati dal server.\n");
            printf("\nPremi invio per continuare...");
            getchar();
            return;
        }
        
        buffer[bytes_received] = '\0';
        
        if (strncmp(buffer, "SEARCH_FAIL", 11) == 0) {
            printf("Nessun film trovato con gli ID specificati.\n");
        } else {
            printf("Film trovati nel carrello:\n\n");
            display_films(buffer, 1);
        }


        int choice = 0;
        // Mostra le opzioni del carrello
        printf("\n\n===== OPZIONI =====\n");
        printf("1. Effettua il Check-out\n");
        printf("2. Svuota Carrello\n");
        printf("3. Torna al menu principale\n");
        printf("Scelta: ");
        if (scanf("%d", &choice) != 1) {
            // Input non valido (non è un numero)
            while (getchar() != '\n'); // Pulisci il buffer di input
            choice = 0; // Imposta una scelta non valida per entrare nel default
        } else {
            getchar(); // Consuma il carattere newline solo se l'input era valido
        }

        switch (choice) {
            case 1:
                check_out(sock);
                printf("\nPremi invio per continuare...");
                getchar();
                break;
            case 2:
                num_film = 0;
                printf("\nCarrello svuotato.");
                printf("\nPremi invio per continuare...");
                getchar();
                break;
            case 3:
                return;
            default:
                printf("Scelta non valida. Riprova.\n");
                printf("\nPremi invio per continuare...");
                getchar();
        
        }
    }
}

void check_out(int sock){
    for (int i = 0; i < num_film; i++)
    {
        char request[BUFFER_SIZE];
        char buffer[BUFFER_SIZE] = {0};
        // Crea la richiesta di login
        snprintf(request, BUFFER_SIZE, "NOLEGGIO:%d:%d", id_utente_loggato, carrello[i]);
        
        // Invia la richiesta al server
        send(sock, request, strlen(request), 0);
        
        // Ricevi la risposta
        read(sock, buffer, BUFFER_SIZE);
        
        // Controlla la risposta del server
        if (strcmp(buffer, "LOAN_OK") == 0) {
            printf("Hai noleggiato il film con ID %d\n", carrello[i]);
        } else if (strcmp(buffer, "LOAN_FAIL") == 0) {
            printf("Errore nel noleggio del film\n");
        } else {
            printf("Errore nella comunicazione con il server: %s\n", buffer);
        }
    }
    num_film = 0; // Svuota il carrello dopo il check-out
    printf("Carrello svuotato dopo il check-out.\n");
}


// Funzione per visualizzare il catalogo
// Funzione per richiedere e visualizzare il catalogo film
void view_catalogo(int sock) {
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 10] = {0};
    int bytes_received;
    
    clear_screen();
    printf("===== CATALOGO FILM =====\n\n");
    
    snprintf(request, BUFFER_SIZE, "CATALOGO");
    send(sock, request, strlen(request), 0);
    
    bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received <= 0) {
        printf("Errore nella ricezione dei dati dal server.\n");
        printf("\nPremi invio per continuare...");
        getchar(); // Attende che l'utente prema invio
        return;
    }
    
    buffer[bytes_received] = '\0';
    
    if (strncmp(buffer, "FILM_FAIL", 9) == 0) {
        printf("Errore: Impossibile recuperare il catalogo film.\n");
    } else {
        display_films(buffer, 1);
    }
}

void display_films(const char *film_data, int options) {
    char *data = strdup(film_data);
    if (data == NULL) {
        printf("Errore: memoria insufficiente.\n");
        return;
    }
    
    char *token;
    char *rest = data;
    int film_count = 0;
    
    token = strtok_r(rest, "\n", &rest);
    if (token != NULL) {
        film_count = atoi(token);
        printf("Trovati %d film:\n\n", film_count);
    }

    if (options == 1)
    {
         // Intestazione della tabella
        printf("+-----+---------------------------+------------------+-------------------+\n");
        printf("| %-3s | %-25s | %-16s | %-17s |\n", "ID", "TITOLO", "GENERE", "COPIE DISPONIBILI");
        printf("+-----+---------------------------+------------------+-------------------+\n");
    }
    else
    {
        printf("+-----+---------------------------+------------------+-------------------+\n");
        printf("| %-3s | %-25s | %-16s | %-17s |\n", "ID", "TITOLO", "GENERE", "DATA RESTITUZIONE");
        printf("+-----+---------------------------+------------------+-------------------+\n");
    }
    
    char id[10] = "";
    char titolo[100] = "";
    char genere[50] = "";
    char ultima_colonna[11] = "";
    
    while ((token = strtok_r(rest, "\n", &rest)) != NULL) {
        if (strcmp(token, "---") == 0) {
            // Stampa il film corrente in formato tabella
            if (titolo[0] != '\0') {
                printf("| %-3s | %-25s | %-16s | %-17s |\n", 
                       id,
                       titolo, 
                       genere, 
                       ultima_colonna);
                
                // Resetta i campi per il prossimo film
                id[0] = '\0';
                titolo[0] = '\0';
                genere[0] = '\0';
                ultima_colonna[0] = '\0';
            }
            continue;
        }
        
        char *key = token;
        char *value = strchr(token, ':');
        
        if (value != NULL) {
            *value = '\0';  // Termina la chiave
            value += 2;     // Salta il ':' e lo spazio
            
            if (strcasecmp(key, "id") == 0) {
                strncpy(id, value, sizeof(id) - 1);
                id[sizeof(id) - 1] = '\0';
            } else if (strcasecmp(key, "titolo") == 0) {
                strncpy(titolo, value, sizeof(titolo) - 1);
                titolo[sizeof(titolo) - 1] = '\0';
            } else if (strcasecmp(key, "genere") == 0) {
                strncpy(genere, value, sizeof(genere) - 1);
                genere[sizeof(genere) - 1] = '\0';
            } else if (options == 1 && strcasecmp(key, "copie_disponibili") == 0) {
                strncpy(ultima_colonna, value, sizeof(ultima_colonna) - 1);
                ultima_colonna[sizeof(ultima_colonna) - 1] = '\0';
            } else if (options == 0 && strcasecmp(key, "data_restituzione") == 0) {
                strncpy(ultima_colonna, value, sizeof(ultima_colonna) - 1);
                ultima_colonna[sizeof(ultima_colonna) - 1] = '\0';
            }
        }
    }
    
    // Stampa l'ultimo film se c'è
    if (titolo[0] != '\0') {
        printf("| %-3s | %-25s | %-16s | %-17s |\n", 
               id, 
               titolo, 
               genere, 
               ultima_colonna);
    }
    
    // Chiudi la tabella
    printf("+-----+---------------------------+------------------+-------------------+\n");
    
    free(data);
}


// Funzione per cercare film per titolo
void search_by_title(int sock) {
    char title[100];
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 20] = {0};
    int bytes_received;
    
    clear_screen();
    printf("===== RICERCA FILM PER TITOLO =====\n\n");
    
    printf("Inserisci parte del titolo: ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = 0; // Rimuovi il newline
    
    // Crea la richiesta di ricerca (1 = ricerca per titolo)
    snprintf(request, BUFFER_SIZE, "SEARCH:1:%s", title);
    send(sock, request, strlen(request), 0);
    
    bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received <= 0) {
        printf("Errore nella ricezione dei dati dal server.\n");
        printf("\nPremi invio per continuare...");
        getchar();
        return;
    }
    
    buffer[bytes_received] = '\0';
    
    if (strncmp(buffer, "SEARCH_FAIL", 11) == 0) {
        printf("Nessun film trovato con il titolo '%s'.\n", title);
    } else {
        printf("Risultati della ricerca per titolo '%s':\n\n", title);
        display_films(buffer, 1);
    }
    
    printf("\nPremi invio per tornare al menu principale...");
    getchar();
}

// Funzione per cercare film per genere
void search_by_genre(int sock) {
    char genre[100];
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 20] = {0};
    int bytes_received;
    
    clear_screen();
    printf("===== RICERCA FILM PER GENERE =====\n\n");
    
    printf("Inserisci genere: ");
    fgets(genre, sizeof(genre), stdin);
    genre[strcspn(genre, "\n")] = 0; // Rimuovi il newline
    
    // Crea la richiesta di ricerca (2 = ricerca per genere)
    snprintf(request, BUFFER_SIZE, "SEARCH:2:%s", genre);
    send(sock, request, strlen(request), 0);
    
    bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received <= 0) {
        printf("Errore nella ricezione dei dati dal server.\n");
        printf("\nPremi invio per continuare...");
        getchar();
        return;
    }
    
    buffer[bytes_received] = '\0';
    
    if (strncmp(buffer, "SEARCH_FAIL", 11) == 0) {
        printf("Nessun film trovato nel genere '%s'.\n", genre);
    } else {
        printf("Risultati della ricerca per genere '%s':\n\n", genre);
        display_films(buffer, 1);
    }
    
    printf("\nPremi invio per tornare al menu principale...");
    getchar();
}

// Funzione per cercare film più popolari
void search_by_popularity(int sock) {
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 20] = {0};
    int bytes_received;
    
    clear_screen();
    printf("===== FILM PIÙ POPOLARI =====\n\n");
    
    // Crea la richiesta di ricerca (3 = ricerca per popolarità)
    // Non è necessario passare una query, quindi usiamo un valore vuoto
    snprintf(request, BUFFER_SIZE, "SEARCH:3:");
    send(sock, request, strlen(request), 0);
    
    bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received <= 0) {
        printf("Errore nella ricezione dei dati dal server.\n");
        printf("\nPremi invio per continuare...");
        getchar();
        return;
    }
    
    buffer[bytes_received] = '\0';
    
    if (strncmp(buffer, "SEARCH_FAIL", 11) == 0) {
        printf("Errore nel recupero dei film più popolari.\n");
    } else {
        printf("Film più popolari (ordinati per numero di noleggi):\n\n");
        display_films(buffer, 1);
    }
    
    printf("\nPremi invio per tornare al menu principale...");
    getchar();
}

void add_to_cart(int sock){
    int id_film = 0;
    printf("Inserisci l'ID del film da aggiungere al carrello: ");

    if (scanf("%d", &id_film) != 1) {
        // Input non valido (non è un numero)
        while (getchar() != '\n'); // Pulisci il buffer di input
        id_film = 0; // Imposta una scelta non valida per entrare nel default
    } else {
        getchar(); // Consuma il carattere newline solo se l'input era valido
    }

    // Controlla se l'ID del film è valido
    if (id_film <= 0) {
        printf("ID film non valido. Riprova.\n");
        printf("\nPremi invio per continuare...");
        getchar();
        return;
    }

    // Controlla se il film è già presente nel carrello
    if (is_film_already_in_cart(id_film)) {
        printf("Il film con ID %d è già presente nel carrello.\n", id_film);
        printf("\nPremi invio per continuare...");
        getchar();
        return;
    }

    // Controlla se il carrello è pieno
    if (num_film < 5) {
        carrello[num_film] = id_film;
        num_film++;
        printf("Film con ID %d aggiunto al carrello.\n", id_film);
    } else {
        printf("Carrello pieno. Non puoi aggiungere più di 5 film.\n");
    }

    printf("\nPremi invio per continuare...");
    getchar();
}

int is_film_already_in_cart(int id_film) {
    for (int i = 0; i < num_film; i++) {
        if (carrello[i] == id_film) {
            return 1; // Film già presente nel carrello
        }
    }
    return 0; // Film non presente nel carrello
}

void retrieve_loaned_films(int sock) {

    while (1)
    {
        clear_screen();
        printf("===== FILM NOLEGGIATI =====\n\n");
        char request[BUFFER_SIZE];
        char buffer[BUFFER_SIZE * 20] = {0};
        int bytes_received;
        
        // Crea la richiesta di ricerca (5 = ricerca film noleggiati utente loggato)
        snprintf(request, BUFFER_SIZE, "SEARCH:5:%d", id_utente_loggato);
        send(sock, request, strlen(request), 0);
        
        bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received <= 0) {
            printf("Errore nella ricezione dei dati dal server.\n");
            printf("\nPremi invio per continuare...");
            getchar();
            return;
        }
        
        buffer[bytes_received] = '\0';
        
        if (strncmp(buffer, "SEARCH_FAIL", 11) == 0) {
            printf("Nessun film noleggiato\n");
            printf("\nPremi invio per tornare al menu principale...");
            getchar();
            return;
        } else {
            display_films(buffer, 0);
        }
    
        int choice = 0;
        // Mostra le opzioni del carrello
        printf("\n\n===== OPZIONI =====\n");
        printf("1. Restituisci film\n");
        printf("2. Torna al menu principale\n");
        printf("Scelta: ");

        if (scanf("%d", &choice) != 1) {
            // Input non valido (non è un numero)
            while (getchar() != '\n'); // Pulisci il buffer di input
            choice = 0; // Imposta una scelta non valida per entrare nel default
        } else {
            getchar(); // Consuma il carattere newline solo se l'input era valido
        }


        switch (choice) {
            case 1:
                // Funzione per restituire un film
                restituisci_film(sock);
                break;
            case 2:
                return;
            default:
                printf("Scelta non valida. Riprova.\n");
                printf("\nPremi invio per continuare...");
                getchar();
        }
    }
}

void restituisci_film(int sock){
    int id_film = 0;
    printf("Inserisci l'ID del film da restituire: ");

    if (scanf("%d", &id_film) != 1) {
        // Input non valido (non è un numero)
        while (getchar() != '\n'); // Pulisci il buffer di input
        id_film = 0; // Imposta una scelta non valida per entrare nel default
    } else {
        getchar(); // Consuma il carattere newline solo se l'input era valido
    }

    // Controlla se l'ID del film è valido
    if (id_film <= 0) {
        printf("ID film non valido. Riprova.\n");
        printf("\nPremi invio per continuare...");
        getchar();
        return;
    }

    // Invia la richiesta di restituzione al server
    char request[BUFFER_SIZE];
    snprintf(request, BUFFER_SIZE, "RESTITUZIONE:%d:%d", id_utente_loggato, id_film);
    send(sock, request, strlen(request), 0);

    char buffer[BUFFER_SIZE] = {0};
    read(sock, buffer, BUFFER_SIZE);

    // Controlla la risposta del server
    if (strcmp(buffer, "RETURN_OK") == 0) {
        printf("Film con ID %d restituito con successo.\n", id_film);
    } else if (strcmp(buffer, "RETURN_FAIL") == 0) {
        printf("Errore nella restituzione del film.\n");
    } else {
        printf("Errore nella comunicazione con il server: %s\n", buffer);
    }

    printf("\nPremi invio per continuare...");
    getchar();
}

// Funzione per mostrare il menu di ricerca film
void show_search_menu(int sock) {
    int choice = 0;
    
    while (1) {
        clear_screen();
        view_catalogo(sock);
        printf("\n===== MENU RICERCA FILM =====\n");
        printf("1. Aggiungi al carrello tramite ID\n");
        printf("2. Cerca per titolo\n");
        printf("3. Cerca per genere\n");
        printf("4. Mostra i film più popolari\n");
        printf("5. Torna al menu principale\n");
        printf("Scelta: ");
        if (scanf("%d", &choice) != 1) {
            // Input non valido (non è un numero)
            while (getchar() != '\n'); // Pulisci il buffer di input
            choice = 0; // Imposta una scelta non valida per entrare nel default
        } else {
            getchar(); // Consuma il carattere newline solo se l'input era valido
        }
        
        switch (choice) {
            case 1:
                add_to_cart(sock);
                break;
            case 2:
                search_by_title(sock);
                break;
            case 3:
                search_by_genre(sock);
                break;
            case 4:
                search_by_popularity(sock);
                break;
            case 5:
                return;
            default:
                printf("Scelta non valida. Riprova.\n");
                printf("\nPremi invio per continuare...");
                getchar();
        }
    }
}