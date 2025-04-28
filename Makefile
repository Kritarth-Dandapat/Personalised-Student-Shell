# Compiler and flags
CC = gcc
CFLAGS = -Wall -std=c99

# Paths
SRC_DIR = src
OBJ_DIR = obj

# Source files and object files
SRC_FILES = $(SRC_DIR)/main.c $(SRC_DIR)/scf.c $(SRC_DIR)/utils.c  # Including both scf.c and utils.c
OBJ_FILES = $(OBJ_DIR)/main.o $(OBJ_DIR)/scf.o $(OBJ_DIR)/utils.o  # Corresponding object files

# Executable name
EXEC = my_shell

# Create object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Target to build the shell when you type 'make shell'
shell: $(OBJ_FILES) 
	$(CC) $(CFLAGS) -pg -o $(EXEC) $(OBJ_FILES)

# Rule for compiling main.c
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(SRC_DIR)/scf.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o

# Rule for compiling scf.c
$(OBJ_DIR)/scf.o: $(SRC_DIR)/scf.c $(SRC_DIR)/scf.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/scf.c -o $(OBJ_DIR)/scf.o

# Rule for compiling utils.c
$(OBJ_DIR)/utils.o: $(SRC_DIR)/utils.c $(SRC_DIR)/scf.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/utils.c -o $(OBJ_DIR)/utils.o

# Clean up object files and executable
clean:
	rm -rf $(OBJ_DIR) $(EXEC)
