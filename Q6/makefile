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
	./$(TARGET) -i UDSSD/home/mayrozen/Downloads/study/OS/OS - EX2/exe6/ttt.cpp

# Rule to run the TCP client connecting to localhost and port 4050
run_client: 
	./mync -o UDSCD/home/mayrozen/Downloads/study/OS/OS - EX2/exe6/ttt.cpp

# Rule to run the datagram server
run_server_dgram: $(TARGET) $(TTT_TARGET)
	./$(TARGET) -id /home/mayrozen/Downloads/study/OS/OS - EX2/exe6/ttt.cpp

# Rule to run the datagram client
run_client_dgram: 
	./$(TARGET) -od /home/mayrozen/Downloads/study/OS/OS - EX2/exe6/ttt.cpp

.PHONY: all clean run_server run_client

clean:
	rm -f $(TARGET) $(TTT_TARGET) $(OBJS) $(TTT_OBJS)
