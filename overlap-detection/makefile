CC=gcc
CPP=g++
CFLAGS+= -m64 -g -Wall
LIBS=
ALLDEP:= $(patsubst %.h,%.o,$(wildcard *.h))
ALLILP:= $(wildcard *_ILP.c)

.PHONY: all
all: $(patsubst %.cpp,%.out,$(filter-out $(patsubst %.h,%.cpp,$(wildcard *.h)), $(wildcard *.cpp)))

.PHONY: product
product: CFLAGS = -O3
product: $(ALLDEP) $(patsubst %.cpp,%.out,$(filter-out $(patsubst %.o,%.cpp,$(ALLDEP)) $(ALLILP), $(wildcard *.cpp)))

sampleFast%.out: sampleFast%.cpp
	$(CPP) $(CFLAGS) -std=c++17 -o $@ $^ $(LIBS)
genSubseqSeeds.out: genSubseqSeeds.cpp $(ALLDEP)
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS) -pthread

%.out: %.cpp $(ALLDEP)
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)
%.out: %.c $(ALLDEP)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
%.o: %.cpp %.h makefile
	$(CPP) $(CFLAGS) -MMD -c $< -o $@
%.o: %.c %.h makefile
	$(CC) $(CFLAGS) -MMD -c $< -o $@

.PHONY: clean

clean:
	rm -rf *.out *.o *.dSYM *.d

.PHONY: realclean clean-backups

realclean: clean clean-backups

clean-backups:
	rm -rf *~ #*#     
