CXX = g++
CXXFLAGS = -std=c++20 -Wall -I../src
LDFLAGS = 

SRC_DIR = ../src
TEST_DIR = .

LIB_OBJS = $(SRC_DIR)/analogDecoder.o

TEST_SRCS = test_analog_decoder.cpp test_digital_decoder.cpp test_main.cpp
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

TEST_BIN = run_tests

.PHONY: all clean test

all: $(TEST_BIN)

$(TEST_BIN): $(TEST_OBJS) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SRC_DIR)/analogDecoder.o: $(SRC_DIR)/analogDecoder.cpp $(SRC_DIR)/analogDecoder.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -f $(TEST_OBJS) $(LIB_OBJS) $(TEST_BIN)
	rm -f $(SRC_DIR)/*.o
