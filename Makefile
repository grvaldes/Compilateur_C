# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source and object files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Executable name
EXEC = $(BUILD_DIR)/regex_parser

# Default target
all: $(EXEC)

# Link the executable
$(EXEC): $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compile .c to .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
