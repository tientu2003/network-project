# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread

# Targets
SERVER = server
CLIENT = client.so

# Source files
SERVER_SRC = server.c
CLIENT_SRC = client.c

# Default target
all: $(SERVER) $(CLIENT)

# Compile server
$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC)

# Compile client
$(CLIENT): $(CLIENT_SRC)
	$(CC) -shared -fPIC -o $(CLIENT) $(CLIENT_SRC) $(CFLAGS)

# Clean build files
clean:
	rm -f $(SERVER) $(CLIENT) *.o

# Run the server
run-server: $(SERVER)
	./$(SERVER) 8080

# Run the client
run-client: $(CLIENT)
	./$(CLIENT) 127.0.0.1 8080
