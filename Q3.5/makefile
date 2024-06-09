CXX = clang++
CXXFLAGS = -std=c++11 -Werror -Wsign-conversion -g
VALGRIND_FLAGS = -v --leak-check=full --show-leak-kinds=all --error-exitcode=99

# Define the executable names
TARGET = mync
TTT_TARGET = ttt

# Define the source files
SRCS = mync.cpp
TTT_SRCS = ttt.cpp

OBJS = $(SRCS:.cpp=.o)
TTT_OBJS = $(TTT_SRCS:.cpp=.o)

all: $(TARGET) $(TTT_TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

$(TTT_TARGET): $(TTT_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TTT_TARGET) $(TTT_OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to run the TCP server on port 4050
run_server: $(TARGET) $(TTT_TARGET)
	./$(TARGET) -bTCP4050 -e "./ttt 123456789"

# Rule to run the TCP client connecting to localhost and port 4050
run_client: $(TARGET) $(TTT_TARGET)
	./$(TARGET) -bTCPlocalhost,4050 -ls "./ttt 123456789"

clean:
	rm -f $(TARGET) $(TTT_TARGET) $(OBJS) $(TTT_OBJS)

.PHONY: all clean run_server run_client
