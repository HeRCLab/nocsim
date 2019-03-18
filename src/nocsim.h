#ifndef NOCSIM_H
#define NOCSIM_H

#include "nocsim_types.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

#define NOCSIM_VERSION_PATCH 1
#define NOCSIM_VERSION_MINOR 0
#define NOCSIM_VERSION_MAJOR 0

/* maximum length of a line in a grid definition */
#define NOCSIM_GRID_LINELEN 256

/* "raw" debug printf */
#ifdef EBUG
#define drprintf(...) do { fprintf(stderr, __VA_ARGS__); } while (0)
#else
#define drprintf(...)
#endif

/* debug printf */
#ifdef EBUG
#define dprintf(...) do { fprintf(stderr, "%s:%d:%s(): ",__FILE__, __LINE__, __func__);\
	     fprintf(stderr, __VA_ARGS__); } while (0)
#else
#define dprintf(...)
#endif

int main(int argc, char** argv);

ll_node* nocsim_grid_parse_file(FILE* stream);

#endif
