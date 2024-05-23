<<<<<<< HEAD
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
=======
# CXX = clang++
# CXXFLAGS = -std=c++11 -Werror -Wsign-conversion -g
# VALGRIND_FLAGS = -v --leak-check=full --show-leak-kinds=all --error-exitcode=99

# SOURCES = mync.cpp 
# OBJECTS = $(SOURCES:.cpp=.o)
# EXECUTABLE = mync

# .PHONY: all clean valgrind

# all: $(EXECUTABLE)

# $(EXECUTABLE): $(OBJECTS)
# 	$(CXX) $(CXXFLAGS) -o $@ $^

# %.o: %.c
# 	$(CXX) $(CXXFLAGS) -c -o $@ $<

# valgrind: $(EXECUTABLE)
# 	valgrind $(VALGRIND_FLAGS) ./$(EXECUTABLE)

# clean:
# 	rm -f $(EXECUTABLE) $(OBJECTS)
>>>>>>> e12ebf9e6fbc378695912424a474562fc7edde01
