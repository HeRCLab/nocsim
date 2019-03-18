CC=cc

# debugging
CFLAGS=-O0 -g -Wall -Wextra -pedantic -std=c99 -DEBUG

# production
# CFLAGS=-O3 -Wall -Wextra -pedantic -std=c99 -Werror

.PHONY: all clean

all: src/nocsim

src/nocsim: src/nocsim.o src/grid.o
	cd src && $(CC) $(CFLAGS) -o ../$@ nocsim.o grid.o

src/nocsim.o: src/nocsim.c src/nocsim.h src/nocsim_types.h
	cd src && $(CC) $(CFLAGS) -c nocsim.c

src/grid.o: src/grid.c src/nocsim.h src/nocsim_types.h
	cd src && $(CC) $(CFLAGS) -c grid.c

clean:
	rm -f src/*.o src/nocsim
