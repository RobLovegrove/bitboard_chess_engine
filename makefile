CXX = c++
CXXFLAGS = -std=c++17 -Wall

# Automatically include all .cpp files in engine/ and movegen/
SRCS = $(wildcard *.cpp) $(wildcard movegen/*.cpp) $(wildcard attacks/*.cpp)
OBJS = $(SRCS:.cpp=.o)
TARGET = chess

# Default build = release
all: release

release: CXXFLAGS += -O3
release: $(TARGET)

debug: CXXFLAGS += -O0 -g -DDEBUG
debug: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)