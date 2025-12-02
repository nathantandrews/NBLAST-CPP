CXX := g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -Wpedantic

TARGET := nblast++

SRC := $(wildcard src/*.cpp)

OBJ := $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run