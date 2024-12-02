# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread

# Targets
SERVER = server
CLIENT = client

# Source files
SERVER_SRC = server.c
CLIENT_SRC = client.c

# Object files
SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

# Default target
all: $(SERVER) $(CLIENT)
# Compile server
$(SERVER): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_OBJ)
	rm -f server.o

# Compile client
$(CLIENT): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_OBJ)
	rm -f client.o


# Compile .o files from .c files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(SERVER) $(CLIENT) *.o

# Run the server
run-server: $(SERVER)
	./$(SERVER) 8080

# Run the client
run-client: $(CLIENT)
	./$(CLIENT) 127.0.0.1 8080
