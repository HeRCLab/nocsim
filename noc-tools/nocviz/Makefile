TOP=..
include ${TOP}/Makefile.config

LIB=		nocviz
LIB_SHARED=	Yes

SRCS=		${SRCS_NOCVIZ} nocviz.o datastore.c operations.c graph.c commands.c node_command.c gui.c ../3rdparty/vec.c graph_widget.c text_widget.c

# TCL wiki seems to suggest that -DUSE_TCL_STUBS and -ltclstubs8.6 are needed,
# but that causes CreateInterp() within datastore to break
# STUBS_CFLAGS?=	-DUSE_TCL_STUBS
STUBS_CFLAGS?=	

CFLAGS+=	${TCL_CFLAGS} ${AGAR_CFLAGS} ${STUBS_CFLAGS} -D_GNU_SOURCE
LIBS+=		${TCL_LIBS} ${AGAR_LIBS}

include ${TOP}/mk/build.lib.mk

show_cflags:
	@echo "${CFLAGS}"

show_libs:
	@echo "${LIBS}"

show_cc:
	@echo "${CC}"
