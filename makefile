# Makefile principale che richiama i makefile nelle sottocartelle

.PHONY: all clean server client

all: server client

server:
	$(MAKE) -C server

client:
	$(MAKE) -C client

clean:
	$(MAKE) -C server clean
	$(MAKE) -C client clean