# Compiler settings
CC = g++
CFLAGS = -Wall -O3 -std=c++17 -fopenmp
LDFLAGS = -Ldependency/lib -lgbwtgraph -lgbwt -lsdsl -lhandlegraph -fopenmp -Wl,-rpath,\$$ORIGIN/dependency/lib

# Directories
INCLUDE_DIR = include/gbz-tool
SRC_DIR = src
BUILD_DIR = build

# Files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
EXECUTABLE = gbztool

# Default target
all: $(BUILD_DIR) $(EXECUTABLE)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link object files to create executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -Idependency/include -c $< -o $@

# Clean up
clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE)

# Phony targets
.PHONY: all clean