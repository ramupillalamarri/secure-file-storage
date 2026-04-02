CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread -lssl -lcrypto

all: server client

server: server/server.c server/auth.c server/crypto.c server/file_ops.c
	$(CC) $(CFLAGS) -o server/server $^

client: client/client.c
	$(CC) $(CFLAGS) -o client/client $^

clean:
	rm -f server/server client/client

.PHONY: all clean