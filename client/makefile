CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lpq -lpthread

client: client.c
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm -f server client

.PHONY: all clean