# Compiler and Flags
CXX := g++
CXXFLAGS := -Wall -Wextra -pedantic -std=c++17 -Iinclude
DEBUG_FLAGS := -g
RELEASE_FLAGS := -O2

# Linker Flags
LDFLAGS := -lncurses

# Project Structure
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
TARGET := $(BIN_DIR)/run

# Source and Object Files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

# Default Build (Release)
.PHONY: all
all: $(TARGET)

# Debug Build
.PHONY: debug
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Release Build
.PHONY: release
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)

# Linking
$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compilation
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Cleaned build files."

# Help
.PHONY: help
help:
	@echo "Makefile Commands:"
	@echo "  make          - Build the project (default: release build)"
	@echo "  make debug    - Build with debugging symbols"
	@echo "  make release  - Build with optimizations"
	@echo "  make clean    - Clean build files"
	@echo "  make help     - Show this help message"

