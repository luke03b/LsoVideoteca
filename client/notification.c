# include "notification.h"

void show_notifications(int sock, int* id_utente_loggato) {
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 20] = {0};
    clear_screen();

    printf("===== NOTIFICHE =====\n");

    snprintf(request, BUFFER_SIZE, "NOTIFICHE:%d", *id_utente_loggato);
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