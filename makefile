# Make Artificial Chemistry
all:
	@echo "Making base..."
	@(cd base; make)
	@echo "Making chemistry..."
	@(cd chemistry; make)
	@echo "Making util..."
	@(cd util; make)
	@echo "Making replicator..."
	@(cd replicator; make)
	@echo "done"

zip:
	@echo "Creating artificial-chemistry.zip file..."
	@/bin/ls -d base/*.h base/*.hpp base/*.cpp base/makefile \
		chemistry/*.h chemistry/*.hpp chemistry/*.cpp chemistry/makefile \
		util/*.h util/*.hpp util/*.cpp util/makefile \
		| zip artificial-chemistry -@
	@echo "done"

clean:
	(cd base; make clean)
	(cd chemistry; make clean)
	(cd util; make clean)
	(cd replicator; make clean)
