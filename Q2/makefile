CXX = clang++
CXXFLAGS = -std=c++11 -Werror -Wsign-conversion -g
VALGRIND_FLAGS = -v --leak-check=full --show-leak-kinds=all --error-exitcode=99

SOURCES = mync.cpp ttt.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLES = mync ttt

.PHONY: all clean valgrind

all: $(EXECUTABLES)

mync: mync.o
	$(CXX) $(CXXFLAGS) -o $@ $^

ttt: ttt.o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

valgrind: $(EXECUTABLES)
	valgrind $(VALGRIND_FLAGS) ./mync
	valgrind $(VALGRIND_FLAGS) ./ttt

clean:
	rm -f $(EXECUTABLES) $(OBJECTS)
