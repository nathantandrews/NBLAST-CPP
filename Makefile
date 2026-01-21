CXX := g++
STD := -std=c++20
WARN := -Wall -Wextra -Wpedantic

BUILDTARGET := nblast++

TESTSRC := test/testScore.cpp
TESTTARGET := testScore

SWCDIR := /scratch/preserve/wayne/FlyWire/Skeletons/brain_and_nerve_cord_skeletons_banc_mirrored_swc

SRC := $(wildcard src/*.cpp)

BUILD ?= release

ifeq ($(BUILD),debug)
    CXXFLAGS := $(STD) $(WARN) -g -Og -DDEBUG
    OBJDIR := obj/debug
else
    CXXFLAGS := $(STD) $(WARN) -O2 -DNDEBUG
    OBJDIR := obj/release
endif

OBJS := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(SRC))

all: $(BUILDTARGET)

$(BUILDTARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $@

$(TESTTARGET): $(TESTSRC) src/Scoring.cpp src/LookUpTable.cpp src/Point.cpp src/Debug.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

debug:
	$(MAKE) BUILD=debug

release:
	$(MAKE) BUILD=release

clean:
	rm -rf obj $(BUILDTARGET)

query: $(BUILDTARGET)
	./$(BUILDTARGET) -q Costa2016/smat.fcwb.tsv $(SWCDIR)/$(QUERY).swc $(SWCDIR)/$(TARGET).swc > query_out.txt 2> query_err.txt

test: $(TESTTARGET)
	./$(TESTTARGET)

.PHONY: all debug release clean run
