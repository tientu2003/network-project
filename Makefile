# Makefile for TCP string processing server and client
CC = gcc
CFLAGS = -Wall
SERVER_SRC = server.c
CLIENT_SRC = client.c
HELPER_SRC = helper.c
SERVER_BIN = server
CLIENT_BIN = client
HELPER_BIN = helper

all: $(SERVER_BIN) $(CLIENT_BIN)

# Compile server
$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER_BIN) $(SERVER_SRC)

# Compile client
$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT_BIN) $(CLIENT_SRC)

# Clean up binaries
clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)
