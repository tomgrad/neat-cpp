CXX=g++
CXXFLAGS = --std=c++17
CXXFLAGS += -g -Wall -pedantic -Wextra


neat : main.o genotype.o
	$(CXX) -lpthread -o $@ $^

#library.o : library.cpp
#	$(CXX) $(CXXFLAGS) -c $<

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o neat

