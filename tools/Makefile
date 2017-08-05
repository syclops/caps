CXXFLAGS = -std=c++11 -g -Wall -Wextra -pedantic
CXX = g++

TARGETS = charcount

all: $(TARGETS)

charcount: charcount.cc
	$(CXX) $(CXXFLAGS) -o $@ charcount.cc

clean:
	rm -rf $(TARGETS)
