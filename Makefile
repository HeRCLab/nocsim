CC=cc

# debugging
CFLAGS=-O0 -g3 -Wall -Wextra -pedantic -std=c99 -DEBUG $(ECFLAGS)

# production
# CFLAGS=-O3 -Wall -Wextra -pedantic -std=c99

CFLAGS+=$$(pkg-config --cflags libbsd 2>/dev/null)
LIBS+=$$(pkg-config --libs libbsd 2>/dev/null)

.PHONY: all clean output.db lint

all: src/nocsim

lint: src/nocsim.db
	ikos-report src/nocsim.db

src/nocsim.db:
	make -C ./ clean && yes | ikos-scan make -C ./

src/nocsim: src/nocsim.o src/grid.o src/util.o src/behaviors.o src/simulation.o src/vec.o
	cd src && $(CC) $(CFLAGS) nocsim.o grid.o util.o behaviors.o simulation.o vec.o $(LIBS) -o ../$@

src/nocsim.o: src/nocsim.c src/nocsim.h src/nocsim_types.h
	cd src && $(CC) $(CFLAGS) $(LIBS) -c nocsim.c

src/grid.o: src/grid.c src/nocsim.h src/nocsim_types.h
	cd src && $(CC) $(CFLAGS) -c grid.c

src/util.o: src/util.c src/nocsim.h src/nocsim_types.h
	cd src && $(CC) $(CFLAGS) -c util.c

src/behaviors.o: src/behaviors.c src/nocsim.h src/nocsim_types.h
	cd src && $(CC) $(CFLAGS) -c behaviors.c

src/simulation.o: src/simulation.c src/nocsim.h src/nocsim_types.h
	cd src && $(CC) $(CFLAGS) -c simulation.c

src/vec.o: src/vec.c src/vec.h
	cd src && $(CC) $(CFLAGS) -c vec.c

clean:
	rm -f src/*.o src/nocsim output.db src/output.db src/nocsim.db *.bc src/*.bc
