CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I../shared -I$(shell brew --prefix googletest)/include
LDFLAGS = -L$(shell brew --prefix googletest)/lib -lgtest -lgtest_main

SRCS = $(wildcard test_*.cpp)
TARGET = run_tests

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
