CXXFLAGS = -g -Wall -Wextra -pedantic -O3
CXX = g++

TARGETS = mafsabuild
MAFSA_OBJS = mafsa.o
MAFSABUILD_OBJS = mafsa.o mafsabuild.o

ALL_OBJS = $(MAFSABUILD_OBJS)

# ----- Make Rules -----

all: $(TARGETS)

mafsabuild: $(MAFSABUILD_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(MAFSABUILD_OBJS)

clean:
	rm -rf $(TARGETS) $(ALL_OBJS) mafsabuild.dSYM

# ----8<----8<----8<----8<----8<----8<----8<----8<----8<----8<----8<----8<----
mafsa.o: mafsa.cc mafsa.h
mafsabuild.o: mafsabuild.cc mafsa.h
