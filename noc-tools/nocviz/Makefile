TOP=..
include ${TOP}/Makefile.config

LIB=		nocviz
LIB_SHARED=	Yes

SRCS=		${SRCS_NOCVIZ} nocviz.c datastore.c operations.c

CFLAGS+=	${TCL_CFLAGS} ${AGAR_CFLAGS} -DUSE_TCL_STUBS
LIBS+=		${TCL_LIBS} ${AGAR_LIBS}

include ${TOP}/mk/build.lib.mk

show_cflags:
	@echo "${CFLAGS}"

show_libs:
	@echo "${LIBS}"

show_cc:
	@echo "${CC}"

