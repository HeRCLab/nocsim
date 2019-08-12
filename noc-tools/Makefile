TOP=.
include ${TOP}/Makefile.config

PROJECT=		"noc-tools"
SUBDIR=			3rdparty nocviz nocsim

CFLAGS=			-std=C11

all: all-subdir

clean: clean-subdir

cleandir: cleandir-subdir

install: install-subdir install-includes install-config

deinstall: deinstall-subdir desintall-includes deinstall-config

depend: depend-subdir

include ${TOP}/mk/build.common.mk
include ${TOP}/mk/build.subdir.mk

.PHONY: all clean cleandir install deinstall depend show_cflags show_libs
