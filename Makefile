# Compiler and flags
CC = gcc

# Colors for pretty output
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[1;34m
RESET = \033[0m

CFLAGS = -std=c2x -O3 `pkg-config --cflags sdl2 SDL2_image SDL2_ttf SDL2_mixer libcjson`
CFLAGS += -g -fsanitize=leak,signed-integer-overflow,alignment,bool,vptr
CFLAGS += -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Wstrict-prototypes
LDFLAGS = `pkg-config --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer libcjson`
LDFLAGS += -fsanitize=address,undefined

SRC_DIR = src
BUILD_DIR = build
TARGET = sobre

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Total count
TOTAL := $(words $(SRCS))
COUNT = $(shell expr $(words $(filter $(SRC_DIR)/%,$(MAKECMDGOALS))) + 1)

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	@echo -e "$(BLUE)[Linking]$(RESET) $@"
	@$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# Compilation
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@printf "$(GREEN)[%2d/%2d]$(RESET) Compiling %s\n" $(shell expr $(shell ls -1 $(BUILD_DIR) 2>/dev/null | wc -l) + 1) $(TOTAL) $<
	@$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	@echo -e "$(YELLOW)[Cleaning]$(RESET)"
	@rm -rf $(BUILD_DIR) $(TARGET)
