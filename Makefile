CC=cc

# debugging
CFLAGS=-O0 -g3 -Wall -Wextra -pedantic -std=c99 -DEBUG $(ECFLAGS)

# production
# CFLAGS=-O3 -Wall -Wextra -pedantic -std=c99

TCL_CFLAGS=-I/usr/include/tcl8.6
TCL_LIBS=-ltcl

CFLAGS+=$$(pkg-config --cflags libbsd 2>/dev/null) $(TCL_CFLAGS)
LIBS+=$$(pkg-config --libs libbsd 2>/dev/null) $(TCL_LIBS) -lreadline

.PHONY: all clean output.db lint

all: src/nocsim

lint: src/nocsim.db
	ikos-report --status-filter error src/nocsim.db

src/nocsim.db:
	make -C ./ clean && yes | ikos-scan make -C ./

src/nocsim: src/nocsim.o src/grid.o src/util.o src/simulation.o src/vec.o src/interp.o
	cd src && $(CC) $(CFLAGS) nocsim.o grid.o util.o simulation.o vec.o interp.o -o ../$@ $(LIBS) 

src/nocsim.o: src/nocsim.c src/nocsim.h src/nocsim_types.h
	cd src && $(CC) $(CFLAGS) $(LIBS) -c nocsim.c

src/grid.o: src/grid.c src/nocsim.h src/nocsim_types.h
	cd src && $(CC) $(CFLAGS) -c grid.c

src/util.o: src/util.c src/nocsim.h src/nocsim_types.h
	cd src && $(CC) $(CFLAGS) -c util.c

src/simulation.o: src/simulation.c src/nocsim.h src/nocsim_types.h
	cd src && $(CC) $(CFLAGS) -c simulation.c

src/vec.o: src/vec.c src/vec.h
	cd src && $(CC) $(CFLAGS) -c vec.c

src/interp.o: src/interp.c src/nocsim.h src/nocsim_types.h
	cd src && $(CC) $(CFLAGS) -c interp.c

clean:
	rm -f src/*.o src/nocsim output.db src/output.db src/nocsim.db *.bc src/*.bc
