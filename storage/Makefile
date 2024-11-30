# Compiler
CC = gcc

# Flags
CFLAGS = -Wall -Wextra -g

# Directories
SRCDIR = .
OBJDIR = obj

# Source and object files
SRCS = $(SRCDIR)/test.c
OBJS = $(OBJDIR)/test.o

# Executable
TARGET = test

# Default rule
all: $(TARGET)

# Build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Build object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -rf $(OBJDIR)

# Run rule
run: all
	./$(TARGET)
