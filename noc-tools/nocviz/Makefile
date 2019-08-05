TOP=..
include ${TOP}/Makefile.config

LIB=		nocviz
LIB_SHARED=	Yes

SRCS=		${SRCS_NOCVIZ} nocviz.c datastore.c operations.c graph.c nocviz_commands.c nocviz_node_command.c

STUBS_CFLAGS?=	-DUSE_TCL_STUBS
CFLAGS+=	${TCL_CFLAGS} ${AGAR_CFLAGS} ${STUBS_CFLAGS} -D_GNU_SOURCE
LIBS+=		${TCL_LIBS} ${AGAR_LIBS}

include ${TOP}/mk/build.lib.mk

show_cflags:
	@echo "${CFLAGS}"

show_libs:
	@echo "${LIBS}"

show_cc:
	@echo "${CC}"

