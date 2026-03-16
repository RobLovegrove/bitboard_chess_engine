# ------------------------
# Compiler and flags
# ------------------------
CXX = c++
CXXFLAGS = -std=c++17 -Wall -I.

# ------------------------
# Directories
# ------------------------
SRC_DIRS := engine programs tests
OBJ_DIR := build/obj
BUILD_DIR := build

# ------------------------
# Engine source files (no main)
# ------------------------
ENGINE_SRCS := $(shell find engine -name "*.cpp")
ENGINE_OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(ENGINE_SRCS))

# ------------------------
# Main engine program
# ------------------------
ENGINE_MAIN := programs/main.cpp
ENGINE_MAIN_OBJ := $(OBJ_DIR)/programs/main.o
ENGINE_EXE := $(BUILD_DIR)/chess

# ------------------------
# Test programs
# ------------------------
TEST_SRCS := $(shell find tests -name "*.cpp")
TEST_OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(TEST_SRCS))
TEST_TARGETS := $(patsubst tests/%.cpp,$(BUILD_DIR)/%,$(TEST_SRCS))

# ------------------------
# All targets
# ------------------------
TARGETS := $(ENGINE_EXE) $(TEST_TARGETS)

# ------------------------
# Default build
# ------------------------
all: release

release: CXXFLAGS += -O3
release: $(TARGETS)

debug: CXXFLAGS += -O0 -g -DDEBUG
debug: $(TARGETS)

# ------------------------
# Build main engine
# ------------------------
$(ENGINE_EXE): $(ENGINE_OBJS) $(ENGINE_MAIN_OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# ------------------------
# Build test executables
# ------------------------
$(BUILD_DIR)/%: $(OBJ_DIR)/tests/%.o $(ENGINE_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# ------------------------
# Generic object rule
# ------------------------
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ------------------------
# Clean
# ------------------------
clean:
	rm -rf $(OBJ_DIR) $(BUILD_DIR)