# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Targets
TARGETS = server client

# Source files
HELPER_SRC = helper.c
SERVER_SRC = server.c
CLIENT_SRC = client.c

# Object files
HELPER_OBJ = $(HELPER_SRC:.c=.o)
SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

# Default target
all: $(TARGETS)

# Build server
server: $(SERVER_OBJ) $(HELPER_OBJ)
	$(CC) $(CFLAGS) -o server $(SERVER_OBJ) $(HELPER_OBJ)

# Build client
client: $(CLIENT_OBJ) $(HELPER_OBJ)
	$(CC) $(CFLAGS) -o client $(CLIENT_OBJ) $(HELPER_OBJ)

# Compile helper.c
$(HELPER_OBJ): $(HELPER_SRC) helper.h
	$(CC) $(CFLAGS) -c $(HELPER_SRC)

# Compile server.c
$(SERVER_OBJ): $(SERVER_SRC) helper.h
	$(CC) $(CFLAGS) -c $(SERVER_SRC)

# Compile client.c
$(CLIENT_OBJ): $(CLIENT_SRC) helper.h
	$(CC) $(CFLAGS) -c $(CLIENT_SRC)

# Clean up
clean:
	rm -f $(TARGETS) *.o

# Phony targets
.PHONY: all clean
