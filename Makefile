# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

# Directories
SRC_DIR = src
BIN_DIR = bin
TEST_DIR = test

# Source and target
SRC = $(SRC_DIR)/main.c
TARGET = $(BIN_DIR)/main

# Default target
all: $(TARGET)

# Rule to build the target
$(TARGET): $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Run the program
run: $(TARGET)
	@./$(TARGET)

# Test the program with a sample input file
test: $(TARGET)
	@mkdir -p $(TEST_DIR)
	@./$(TARGET) < $(TEST_DIR)/input.txt

# Clean up build files
clean:
	rm -rf $(BIN_DIR)

# PHONY targets to avoid conflicts with files named 'clean', 'run', or 'test'
.PHONY: all run test clean
