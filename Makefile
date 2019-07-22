CC=cc

TCL_CFLAGS=-I/usr/include/tcl8.6
TCL_LIBS=-ltcl

TCL_FILES=$(shell ls -1 src/nocsim_tcl/*.tcl | grep -v pkgIndex.tcl)

HEADERS=$(shell ls -1 src/*.h) src/nocsim_constants.h

# installation info
PREFIX=/usr/local
BIN_DIR=$(PREFIX)/bin
LIB_DIR=$(PREFIX)/lib
TCL_DIR=$(LIB_DIR)/tcltk

# constants
NOCSIM_VERSION_PATCH=0
NOCSIM_VERSION_MINOR=0
NOCSIM_VERSION_MAJOR=1

ifeq ($(DEVELOP),true)
	CFLAGS=-O0 -g3 -Wall -Wextra -pedantic -std=c99 -DEBUG $(ECFLAGS)
	TCL_DEV_DIR=$(shell pwd)/build/
else
	CFLAGS=-O3 -Wall -Wextra -pedantic -std=c99 $(ECFLAGS)
	TCL_DEV_DIR=$(TCL_DIR)
	DEVELOP=false
endif

# libbsd and TCL
CFLAGS+=$$(pkg-config --cflags libbsd 2>/dev/null) $(TCL_CFLAGS)
LIBS+=$$(pkg-config --libs libbsd 2>/dev/null) $(TCL_LIBS) -lreadline


.PHONY: build all clean output.db lint info install gui

info:
	@echo "installation prefix . . . $(PREFIX)"
	@echo "BIN_DIR . . . . . . . . . $(BIN_DIR)"
	@echo "LIB_DIR . . . . . . . . . $(LIB_DIR)"
	@echo "TCL_DIR . . . . . . . . . $(TCL_DIR)"
	@echo "CC  . . . . . . . . . . . $(CC)"
	@echo "CFLAGS  . . . . . . . . . $(CFLAGS)"
	@echo "LIBS  . . . . . . . . . . $(LIBS)"
	@echo "TCL library files . . . . $(TCL_FILES)"
	@echo "HEADERS . . . . . . . . . $(HEADERS)"
	@echo "development build?  . . . $(DEVELOP)"
	@echo "execute 'make all' to build, and 'make install' to install to the specified prefix"
	@echo "for development builds, use 'make DEVELOP=true ...'"
	@echo "to also install the GUI, run 'make gui' before 'make build'"

all: build

build: src/nocsim src/nocsim_tcl/pkgIndex.tcl 
	rm -rf ./build
	mkdir -p ./build/bin
	mkdir -p ./build/NocsimTCL
	cp src/nocsim build/bin
	@if [ -e gui/nocsim-gui ] ; then cp gui/nocsim-gui build/bin ; else echo "heads up: if you want GUI support, you should run 'make gui', then re-run 'make build'" ; fi
	cp -r src/nocsim_tcl build/NocsimTCL

install: build
	mkdir -p "$(BIN_DIR)"
	mkdir -p "$(TCL_DIR)"
	cp build/bin/* "$(BIN_DIR)/"
	cp -r build/NocsimTCL "$(TCL_DIR)"

gui: gui/nocsim-gui

gui/nocsim-gui:
	@if [ ! -e gui/config.status ] ; then echo "ERROR: you need to run 'cd gui && ./configure'" ; fi
	test -e gui/config.status
	make -C gui

lint: src/nocsim.db
	ikos-report --status-filter error src/nocsim.db

src/nocsim.db:
	make -C ./ clean && yes | ikos-scan make -C ./ build

src/nocsim: src/nocsim.o src/grid.o src/util.o src/simulation.o src/vec.o src/interp.o
	cd src && $(CC) $(CFLAGS) nocsim.o grid.o util.o simulation.o vec.o interp.o -o ../$@ $(LIBS)

src/nocsim_constants.h:
	rm -f src/nocsim_constants.h
	echo "#ifndef NOCSIM_CONSTANTS_H" >> $@
	echo "#define NOCSIM_CONSTANTS_H" >> $@
	echo "#define NOCSIM_VERSION_PATCH $(NOCSIM_VERSION_PATCH)" >> $@
	echo "#define NOCSIM_VERSION_MINOR $(NOCSIM_VERSION_MINOR)" >> $@
	echo "#define NOCSIM_VERSION_MAJOR $(NOCSIM_VERSION_MAJOR)" >> $@
	echo "#define NOCSIM_TCL_PATH \"$(TCL_DIR)\"" >> $@
	echo "#define NOCSIM_TCL_DEV_PATH \"$(TCL_DEV_DIR)\"" >> $@
	echo "#endif" >> $@

src/nocsim.o: src/nocsim.c $(HEADERS)
	cd src && $(CC) $(CFLAGS) $(LIBS) -c nocsim.c

src/grid.o: src/grid.c $(HEADERS)
	cd src && $(CC) $(CFLAGS) -c grid.c

src/util.o: src/util.c $(HEADERS)
	cd src && $(CC) $(CFLAGS) -c util.c

src/simulation.o: src/simulation.c $(HEADERS)
	cd src && $(CC) $(CFLAGS) -c simulation.c

src/vec.o: src/vec.c src/vec.h
	cd src && $(CC) $(CFLAGS) -c vec.c

src/interp.o: src/interp.c $(HEADERS)
	cd src && $(CC) $(CFLAGS) -c interp.c

src/nocsim_tcl/pkgIndex.tcl: $(TCL_FILES)
	rm -f src/nocsim_tcl/pkgIndex.tcl
	cd src/nocsim_tcl/ && (printf "pkg_mkIndex . $$(ls -1 *.tcl)\n" | tclsh)

clean:
	rm -f src/*.o src/nocsim output.db src/output.db src/nocsim.db *.bc src/*.bc src/nocsim_tcl/pkgIndex.tcl
	rm -rf build
	if [ -e gui/config.status ] ; then make -C gui clean ; fi
