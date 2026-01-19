CXX := g++
STD := -std=c++20
WARN := -Wall -Wextra -Wpedantic

TARGET := nblast++

SRC := $(wildcard src/*.cpp)

# Default build mode
BUILD ?= release

ifeq ($(BUILD),debug)
    CXXFLAGS := $(STD) $(WARN) -g -Og -DDEBUG
    OBJDIR := obj/debug
else
    CXXFLAGS := $(STD) $(WARN) -O2 -DNDEBUG
    OBJDIR := obj/release
endif

OBJS := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $@

debug:
	$(MAKE) BUILD=debug

release:
	$(MAKE) BUILD=release

clean:
	rm -rf obj $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all debug release clean run
