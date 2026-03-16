CXX = c++
CXXFLAGS = -std=c++17 -Wall -I.  # include current directory for headers

# ------------------------
# Engine source files (no main)
# ------------------------
ENGINE_SRCS := $(shell find engine -name "*.cpp")
ENGINE_OBJS := $(ENGINE_SRCS:.cpp=.o)

# ------------------------
# Main engine program
# ------------------------
ENGINE_MAIN := programs/main.cpp
ENGINE_MAIN_OBJ := programs/main.o
ENGINE_EXE := bin/chess   # keep old "chess" name

# ------------------------
# Test programs
# ------------------------
TEST_SRCS := $(shell find tests -name "*.cpp")
TEST_OBJS := $(TEST_SRCS:.cpp=.o)
TEST_TARGETS := $(patsubst tests/%.cpp,bin/%,$(TEST_SRCS))  # outputs in bin/

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
# Link main engine
# ------------------------
$(ENGINE_EXE): $(ENGINE_OBJS) $(ENGINE_MAIN_OBJ)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $^

# ------------------------
# Link test programs (do NOT include main.o)
# ------------------------
bin/%: tests/%.o $(ENGINE_OBJS)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $^

# ------------------------
# Generic compilation rule
# ------------------------
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ------------------------
# Clean everything
# ------------------------
clean:
	rm -f $(ENGINE_OBJS) $(ENGINE_MAIN_OBJ) $(TEST_OBJS) $(TARGETS)