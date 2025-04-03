#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <libpq-fe.h>
#include "client_handlers.h"
#include "auth.h"

#define PORT 8080
#define MAX_CLIENTS 10

void clear_screen() {
    printf("\033[H\033[J");
}

int main() {
    clear_screen();
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t threads[MAX_CLIENTS];
    int thread_count = 0;
    
    // Connessione al database PostgreSQL
    PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=auth_db user=postgres password=postgres");
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connessione al database fallita: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
    
    printf("Connessione al database PostgreSQL riuscita\n");
    
    // Creazione socket del server
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Creazione socket fallita");
        exit(EXIT_FAILURE);
    }
    
    // Opzione per riutilizzare la porta
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt fallito");
        exit(EXIT_FAILURE);
    }
    
    // Configurazione dell'indirizzo del server
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Binding del socket alla porta specificata
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Binding fallito");
        exit(EXIT_FAILURE);
    }
    
    // Server in ascolto
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Errore durante l'ascolto");
        exit(EXIT_FAILURE);
    }
    
    printf("Server in ascolto sulla porta %d\n", PORT);
    
    // Accettazione di connessioni in loop
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accettazione fallita");
            continue;
        }
        
        printf("Nuova connessione accettata\n");
        
        // Creazione di un nuovo thread per gestire il client
        thread_args *args = malloc(sizeof(thread_args));
        args->socket = new_socket;
        args->conn = conn;
        
        if (pthread_create(&threads[thread_count], NULL, handle_client, (void *)args) != 0) {
            perror("Creazione thread fallita");
            close(new_socket);
            free(args);
        } else {
            thread_count++;
            printf("Client connesso\n");
        }
        
        // Pulizia dei thread terminati
        for (int i = 0; i < thread_count; i++) {
            if (pthread_detach(threads[i]) == 0) {
                // Thread terminato
            }
        }
    }
    
    // Chiusura della connessione al database
    PQfinish(conn);
    
    return 0;
}