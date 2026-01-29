CXX := g++
STD := -std=c++20
WARN := -Wall -Wextra -Wpedantic

BUILD_TARGET := nblast++

TEST_SRC := test/testScore.cpp
TEST_TARGET := testScore

BANC := /scratch/preserve/wayne/FlyWire/Skeletons/banc_mirrored
FAFB := /scratch/preserve/wayne/FlyWire/Skeletons/fafb_banc_space/banc_space_swc/elastix_tpsreg_240721

ifeq ($(QD),b)
	QUERY_DIR?=$(BANC)
else
	QUERY_DIR?=$(FAFB)
endif
ifeq ($(TD),b)
	TARGET_DIR?=$(BANC)
else
	TARGET_DIR?=$(FAFB)
endif

SRC := $(wildcard src/*.cpp)

BUILD ?= release

ifeq ($(BUILD),debug)
    CXXFLAGS := $(STD) $(WARN) -g -Og -DDEBUG
    OBJ_DIR := obj/debug
else
    CXXFLAGS := $(STD) $(WARN) -O2 -DNDEBUG
    OBJ_DIR := obj/release
endif

OBJS := $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

all: $(BUILD_TARGET)

$(BUILD_TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_TARGET): $(TEST_SRC) src/Scoring.cpp src/LookUpTable.cpp src/Point.cpp src/Debug.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

debug:
	$(MAKE) BUILD=debug

release:
	$(MAKE) BUILD=release

clean:
	rm -rf obj $(BUILD_TARGET) $(TEST_TARGET) fafb-to-banc-err.txt fafb-to-banc.txt

query: $(BUILD_TARGET)
	./$(BUILD_TARGET) -q Costa2016/smat.fcwb.tsv $(QUERY_DIR)/$(QUERY).swc $(TARGET_DIR)/$(TARGET).swc >> $(QUERY_OUT) 2>> $(QUERY_ERR)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

.PHONY: all debug release clean run
