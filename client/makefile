CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lpq -lpthread

client: client.c ui.c auth.c film.c loan.c notification.c
	$(CC) $(CFLAGS) -o client client.c ui.c auth.c film.c loan.c notification.c

clean:
	rm -f server client

.PHONY: all clean