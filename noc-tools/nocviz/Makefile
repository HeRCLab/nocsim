TOP=..
include ${TOP}/Makefile.config

LIB=		nocviz
LIB_SHARED=	Yes

SRCS=		${SRCS_NOCVIZ} nocviz.c datastore.c operations.c

STUBS_CFLAGS?=	-DUSE_TCL_STUBS
CFLAGS+=	${TCL_CFLAGS} ${AGAR_CFLAGS} ${STUBS_CFLAGS}
LIBS+=		${TCL_LIBS} ${AGAR_LIBS}

include ${TOP}/mk/build.lib.mk

show_cflags:
	@echo "${CFLAGS}"

show_libs:
	@echo "${LIBS}"

show_cc:
	@echo "${CC}"

