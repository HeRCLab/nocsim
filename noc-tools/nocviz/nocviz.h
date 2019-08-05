#ifndef NOCVIZ_H
#define NOCVIZ_H

#include <agar/core.h>
#include <agar/gui.h>

#include <stdlib.h>
#include <string.h>

#include <tcl.h>

#include "commands.h"
#include "graph.h"

int DLLEXPORT Nocviz_Init(Tcl_Interp *interp);

#endif
