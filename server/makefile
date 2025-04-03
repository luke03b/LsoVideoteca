CC = gcc
CFLAGS = -Wall -g -I/usr/include/postgresql
LDFLAGS = -lpq -lpthread

server: server.c client_handlers.c auth.c
	$(CC) $(CFLAGS) -o server server.c client_handlers.c auth.c $(LDFLAGS)

clean:
	rm -f server client

.PHONY: all clean