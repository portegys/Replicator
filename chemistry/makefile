# Build the chemistry objects.

CC = gcc

CCFLAGS = -O -DUNIX

all: Neighborhood.o Reaction.o Chemistry.o

Neighborhood.o: Neighborhood.hpp Neighborhood.cpp ../base/Parameters.h
	$(CC) $(CCFLAGS) -c Neighborhood.cpp

Reaction.o: Reaction.hpp Reaction.cpp ../base/Parameters.h
	$(CC) $(CCFLAGS) -c Reaction.cpp

Chemistry.o: Chemistry.hpp Chemistry.cpp ../base/Parameters.h
	$(CC) $(CCFLAGS) -c Chemistry.cpp

clean:
	/bin/rm -f *.o

