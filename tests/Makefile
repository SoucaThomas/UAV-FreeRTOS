CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I../shared

# Use brew paths on macOS, system paths on Linux
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
  CXXFLAGS += -I$(shell brew --prefix googletest)/include
  LDFLAGS = -L$(shell brew --prefix googletest)/lib -lgtest -lgtest_main
else
  LDFLAGS = -lgtest -lgtest_main -pthread
endif

SRCS = $(wildcard test_*.cpp)
TARGET = run_tests

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
