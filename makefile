# ------------------------
# Compiler and flags
# ------------------------
CXX = c++
CXXFLAGS = -std=c++17 -Wall -I. -Isrc

# ------------------------
# Directories
# ------------------------
SRC_DIRS := src engine programs tests
OBJ_DIR := build/obj
BUILD_DIR := build

# ------------------------
# Engine source files (no main)
# ------------------------
ENGINE_SRCS := $(shell find src -name "*.cpp")
ENGINE_OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(ENGINE_SRCS))

# ------------------------
# Programs (main entry points)
# ------------------------
PROGRAMS := uci bench
PROGRAM_SRCS := $(patsubst %,programs/%.cpp,$(PROGRAMS))
PROGRAM_OBJS := $(patsubst programs/%.cpp,$(OBJ_DIR)/programs/%.o,$(PROGRAM_SRCS))
PROGRAM_EXES := $(patsubst programs/%.cpp,$(BUILD_DIR)/%,$(PROGRAM_SRCS))

# ------------------------
# Test programs
# ------------------------
TEST_SRCS := $(shell find tests -name "*.cpp")
TEST_OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(TEST_SRCS))
TEST_TARGETS := $(patsubst tests/%.cpp,$(BUILD_DIR)/%,$(TEST_SRCS))

# ------------------------
# All targets
# ------------------------
TARGETS := $(PROGRAM_EXES) $(TEST_TARGETS)

# ------------------------
# Default build
# ------------------------
all: release

release: CXXFLAGS += -O3
release: $(TARGETS)

debug: CXXFLAGS += -O0 -g -DDEBUG
debug: $(TARGETS)

# ------------------------
# Build program executables (uci, bench, etc.)
# ------------------------
$(BUILD_DIR)/%: programs/%.cpp $(ENGINE_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# ------------------------
# Build test executables
# ------------------------
$(BUILD_DIR)/%: $(OBJ_DIR)/tests/%.o $(ENGINE_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# ------------------------
# Generic object compilation
# ------------------------
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ------------------------
# Clean
# ------------------------
clean:
	rm -rf $(OBJ_DIR) $(BUILD_DIR)