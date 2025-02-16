# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -O2

# Source files
SRC = src/main.c

# Output executable
OUTPUT = zero_out_disk

# Default target
all: $(OUTPUT)

# Build the executable
$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC)

# Clean up
clean:
	rm -f $(OUTPUT)