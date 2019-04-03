#ifndef NOCSIM_H
#define NOCSIM_H

#ifndef __OpenBSD__
#define _POSIX_C_SOURCE 2
#endif

#include "nocsim_types.h"

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

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

/* convert a linked list node containing a node to just the node */
#define ll2node(node) ((nocsim_node*) (node->data))

#define ll2meta(node) ((nocsim_meta*) node->data)

#define foreach_element(cursor, head) for (cursor = head->next; cursor != NULL; cursor = cursor->next)

#define alloc(size, target) do { \
	if ((target = malloc(size)) == NULL) { \
		err(1, "could not allocate memory"); \
	} } while(0)

int main(int argc, char** argv);

ll_node* nocsim_grid_parse_file(FILE* stream);
void nocsim_grid_parse_router(char* def, ll_node* head);
void nocsim_grid_parse_PE(char* def, ll_node* head);
void nocsim_grid_parse_link(char* def, ll_node* head);
void nocsim_grid_parse_behavior(char* def, ll_node* head);
void nocsim_grid_parse_config(char* def, nocsim_meta* meta);

nocsim_node* nocsim_allocate_node(nocsim_node_type type, unsigned int row, unsigned int col, char* id);
char* nocsim_fmt_node(nocsim_node* node);
void nocsim_print_node(FILE* stream, nocsim_node* node);
void nocsim_dump_graphviz(FILE* stream, ll_node* head);
void nocsim_append_ll(ll_node* head, void* data);
unsigned char with_P(float P);
unsigned int randrange(unsigned int lower, unsigned int upper);

void nocsim_behavior_DOR(nocsim_node* node);
void nocsim_DOR_one(nocsim_node* node, nocsim_flit* flit);

void nocsim_step(ll_node* head);
void nocsim_inject(ll_node* head, nocsim_node* from);
void nocsim_handle_arrival(ll_node* cursor, nocsim_direction dir);

#endif
