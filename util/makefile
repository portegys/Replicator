# Build the utility objects.

CC = gcc

CCFLAGS = -O -DUNIX

all: Log.o Random.o

Log.o: Log.hpp Log.cpp ../base/Parameters.h
	$(CC) $(CCFLAGS) -c Log.cpp

Random.o: Random.hpp Random.cpp ../base/Parameters.h
	$(CC) $(CCFLAGS) -c Random.cpp

clean:
	/bin/rm -f *.o

