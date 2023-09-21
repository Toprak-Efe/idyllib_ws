.PHONY: all clean

# Compiler and flags
CXX := g++
CXXFLAGS := -pthread -std=c++17 -g

# Source files
SRC_DIR := src
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

# Output directory and target
BIN_DIR := bin
TARGET := $(BIN_DIR)/main

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -rf $(BIN_DIR)/*