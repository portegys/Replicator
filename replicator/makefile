# Build the Replicator program.

CC = gcc

CCFLAGS = -O -DUNIX

all: Replicator
	@(cd ../base; make)
	@(cd ../chemistry; make)
	@(cd ../util; make)

Replicator: Replicator.o ../base/*.o ../chemistry/*.o ../util/*.o
	$(CC) $(CCFLAGS) -o Replicator Replicator.o \
		../base/*.o ../chemistry/*.o \
		../util/Log.o ../util/Random.o \
		 -lm -lglut -lstdc++

Replicator.o: Replicator.cpp ../base/Parameters.h ../chemistry/*.hpp
	$(CC) $(CCFLAGS) -c Replicator.cpp

clean:
	@/bin/rm -f *.o

