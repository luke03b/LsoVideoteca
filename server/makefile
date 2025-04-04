CC = gcc
CFLAGS = -Wall -g -I/usr/include/postgresql
LDFLAGS = -lpq -lpthread

server: server.c client_handlers.c auth.c film.c loan.c notification.c
	$(CC) $(CFLAGS) -o server server.c client_handlers.c auth.c film.c loan.c notification.c $(LDFLAGS)

clean:
	rm -f server client

.PHONY: all clean