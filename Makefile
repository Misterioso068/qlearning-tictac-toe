CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -O2
SRC = src/main.c
BUILD_DIR = build
TARGET = $(BUILD_DIR)/tictactoe

.PHONY: all clean

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -rf $(BUILD_DIR)