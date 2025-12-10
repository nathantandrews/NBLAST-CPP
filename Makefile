CXX := g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra # -Wpedantic

TARGET := nblast++
BINDIR := bin
OBJDIR := _objs

SRC := $(wildcard src/*.cpp)

OBJS := $(SRC:.cpp=.o)

all: setup $(TARGET)

setup:
	mkdir -p $(OBJDIR) $(BINDIR)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR) $(BINDIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run