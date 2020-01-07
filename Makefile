CXX=g++
CXXFLAGS = --std=c++17
CXXFLAGS += -g -Wall -pedantic -Wextra


main : main.o
	$(CXX) -lpthread -o $@ $^

#library.o : library.cpp
#	$(CXX) $(CXXFLAGS) -c $<

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o main

