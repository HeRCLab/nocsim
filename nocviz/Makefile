TOP=..
include ${TOP}/Makefile.config

LIB=		nocviz
LIB_SHARED=	Yes

SRCS=		${SRCS_NOCVIZ} nocviz.o datastore.c operations.c graph.c commands.c node_command.c gui.c ../3rdparty/vec.c graph_widget.c text_widget.c link_command.c graph_logic.c

# TCL wiki seems to suggest that -DUSE_TCL_STUBS and -ltclstubs8.6 are needed,
# but that causes CreateInterp() within datastore to break
# STUBS_CFLAGS?=	-DUSE_TCL_STUBS
STUBS_CFLAGS?=	

CFLAGS+=	${TCL_CFLAGS} ${AGAR_CFLAGS} ${AGAR_MATH_CFLAGS} ${STUBS_CFLAGS} -D_GNU_SOURCE
LIBS+=		${TCL_LIBS} ${AGAR_MATH_LIBS} ${AGAR_LIBS}

EXTRA_TARGETS+=	pkgIndex.tcl
CLEANFILES+=	pkgIndex.tcl

include ${TOP}/mk/build.lib.mk

show_cflags:
	@echo "${CFLAGS}"

show_libs:
	@echo "${LIBS}"

show_cc:
	@echo "${CC}"

pkgIndex.tcl: libnocviz.so
	printf "pkg_mkIndex -verbose . $(echo *.tcl *.so)\n" | tclsh
