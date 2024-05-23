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