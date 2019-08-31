# Build the base objects.

CC = gcc

CCFLAGS = -O -DUNIX

all: Automaton.o Bond.o Orientation.o Particle.o Physics.o 

Automaton.o: Automaton.hpp Automaton.cpp Parameters.h
	$(CC) $(CCFLAGS) -c Automaton.cpp
	
Bond.o: Bond.hpp Bond.cpp Parameters.h
	$(CC) $(CCFLAGS) -c Bond.cpp

Orientation.o: Orientation.hpp Orientation.cpp Parameters.h
	$(CC) $(CCFLAGS) -c Orientation.cpp

Particle.o: Particle.hpp Particle.cpp Parameters.h
	$(CC) $(CCFLAGS) -c Particle.cpp
	
Physics.o: Physics.hpp Physics.cpp Parameters.h
	$(CC) $(CCFLAGS) -c Physics.cpp

clean:
	/bin/rm -f *.o
