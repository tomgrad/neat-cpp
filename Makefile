CXX=g++
CXXFLAGS = --std=c++17 -O2
CXXFLAGS += -g -Wall -pedantic -Wextra


neat : main.o genotype.o pool.o
	$(CXX) -lpthread -o $@ $^

test : test.o genotype.o pool.o
	$(CXX) -lpthread -o $@ $^

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o neat

