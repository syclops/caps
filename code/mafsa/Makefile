CXXFLAGS = -std=c++11 -g -Wall -Wextra -pedantic
CXX = g++

TARGETS = mafsabuild mafsatest
MAFSA_OBJS = mafsa.o
MAFSABUILD_OBJS = mafsa.o mafsabuild.o
MAFSATEST_OBJS = mafsa.o mafsatest.o

ALL_OBJS = $(MAFSABUILD_OBJS) $(MAFSATEST_OBJS)

# ----- Make Rules -----

all: $(TARGETS)

mafsabuild: $(MAFSABUILD_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(MAFSABUILD_OBJS)

mafsatest: $(MAFSATEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(MAFSATEST_OBJS)

clean:
	rm -rf $(TARGETS) $(ALL_OBJS) *.dSYM

# ----8<----8<----8<----8<----8<----8<----8<----8<----8<----8<----8<----8<----
mafsa.o: mafsa.cc mafsa.h
mafsabuild.o: mafsabuild.cc mafsa.h
mafsatest.o: mafsatest.cc mafsa.h
