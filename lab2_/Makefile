CXX = g++
CXXFLAGS = -O2 -std=c++11

SRCS = sort_compare.cpp bubble_sort.cpp insertion_sort.cpp merge_sort.cpp quick_sort.cpp heap_sort.cpp radix_sort.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = sort_compare

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
