CXX = c++
CXXFLAGS = -std=c++17 -Wall

SRCS := $(shell find . -name "*.cpp")
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

TARGET = chess

all: release

release: CXXFLAGS += -O3
release: $(TARGET)

debug: CXXFLAGS += -O0 -g -DDEBUG
debug: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)