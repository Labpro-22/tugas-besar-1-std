# Makefile for C++ OOP Project (Optimized & Recursive)

# Compiler settings
CXX      := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -I include

# Directories
SRC_DIR     := src
OBJ_DIR     := build
BIN_DIR     := bin
INCLUDE_DIR := include
DATA_DIR    := data
CONFIG_DIR  := config

# Target executable
TARGET := $(BIN_DIR)/game

# 1. Recursive Source Finding
# Secara otomatis mencari semua file .cpp di dalam src/ dan semua sub-foldernya
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')

# 2. Dynamic Object Mapping
# Mengubah path src/xxx/yyy.cpp menjadi build/xxx/yyy.o
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Main targets
all: directories $(TARGET)

# Create necessary root directories
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR) $(CONFIG_DIR)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Build successful! Executable is at $(TARGET)"

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the game
run: all
	./$(TARGET)

# ----------------------------------------------------------------------
# Tests (save/load/config/logger regression suite)
# ----------------------------------------------------------------------
TEST_SRC      := tests/run_save_load_tests.cpp
TEST_TARGET   := $(BIN_DIR)/run_save_load_tests
# Same set of objects as the game, minus main.o, plus the test driver.
LIB_OBJS      := $(filter-out $(OBJ_DIR)/main.o, $(OBJS))

$(TEST_TARGET): $(LIB_OBJS) $(TEST_SRC) | directories
	$(CXX) $(CXXFLAGS) $(LIB_OBJS) $(TEST_SRC) -o $@
	@echo "Test build successful! Executable is at $(TEST_TARGET)"

tests: $(TEST_TARGET)
	./$(TEST_TARGET)

# Clean up generated files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Cleaned up $(OBJ_DIR) and $(BIN_DIR)"

# Rebuild everything from scratch
rebuild: clean all

.PHONY: all clean rebuild run directories tests