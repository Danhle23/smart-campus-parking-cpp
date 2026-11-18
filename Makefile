CXX ?= c++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Iinclude
DEBUG_FLAGS := -O0 -g
SAN_FLAGS := -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer

CORE_SOURCES := src/DataLoader.cpp src/FeeCalculator.cpp src/FuzzySearch.cpp \
	src/ParkingGraph.cpp src/ParkingSystem.cpp src/Types.cpp
APP_SOURCES := $(CORE_SOURCES) app/main.cpp
TEST_SOURCES := $(CORE_SOURCES) tests/test_main.cpp

.PHONY: all test demo sanitize clean

all: build/smart_parking build/smart_parking_tests

build:
	mkdir -p build

build/smart_parking: $(APP_SOURCES) | build
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) $(APP_SOURCES) -o $@

build/smart_parking_tests: $(TEST_SOURCES) | build
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) $(TEST_SOURCES) -o $@

test: build/smart_parking_tests
	./build/smart_parking_tests

demo: build/smart_parking
	./build/smart_parking --demo

sanitize: | build
	$(CXX) $(CXXFLAGS) $(SAN_FLAGS) $(TEST_SOURCES) -o build/smart_parking_tests_san
	ASAN_OPTIONS=halt_on_error=1 ./build/smart_parking_tests_san

clean:
	rm -f build/smart_parking build/smart_parking_tests build/smart_parking_tests_san
