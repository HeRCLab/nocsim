#ifndef NOCSIM_H
#define NOCSIM_H

#ifndef __OpenBSD__
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#endif

#include "nocsim_types.h"
#include "vec.h"

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <tcl.h>

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
#define dbprintf(...) do { fprintf(stderr, "%s:%d:%s(): ",__FILE__, __LINE__, __func__);\
	     fprintf(stderr, __VA_ARGS__); } while (0)
#else
#define dbprintf(...)
#endif

#ifdef EBUG
#define dbprint_node(node) do { nocsim_print_node(stderr, node); } while(0)
#else
#define dbprint_node(node)
#endif

#define alloc(size, target) do { \
	if ((target = malloc(size)) == NULL) { \
		err(1, "could not allocate memory"); \
	} } while(0)

#define UNUSED(x) (void)(x)

int main(int argc, char** argv);

void nocsim_grid_create_router(nocsim_state* state, char* id, unsigned int row, unsigned int col, char* behavior);
void nocsim_grid_create_PE(nocsim_state* state, char* id, unsigned int row, unsigned int col, char* behavior);
void nocsim_grid_create_link(nocsim_state* state, char* from_id, char* to_id);
void nocsim_grid_config(nocsim_state* state, char* key, char* val);

void nocsim_init_node(nocsim_node* n, nocsim_node_type type, unsigned int row, unsigned int col, char* id);
char* nocsim_fmt_node(nocsim_node* node);
void nocsim_print_node(FILE* stream, nocsim_node* node);
void nocsim_dump_graphviz(FILE* stream, nocsim_state* state);
unsigned char with_P(float P);
unsigned int randrange(unsigned int lower, unsigned int upper);

void nocsim_behavior_DOR(nocsim_node* node);
void nocsim_behavior_ADOR(nocsim_node* node);
void nocsim_DOR_one(nocsim_node* node, nocsim_flit* flit);

void nocsim_step(nocsim_state* state);
void nocsim_inject(nocsim_state* state, nocsim_node* from);
void nocsim_handle_arrival(nocsim_node* state, nocsim_direction dir);

void nocsim_interp(FILE* stream);

#endif
