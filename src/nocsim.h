#ifndef NOCSIM_H
#define NOCSIM_H

#include "nocsim_types.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <time.h>
#include <limits.h>

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
#define NOCSIM_LL2N(node) ((nocsim_node*) (node->data))

#define foreach_element(cursor, head) for (cursor = head->next; cursor != NULL; cursor = cursor->next)

int main(int argc, char** argv);

ll_node* nocsim_grid_parse_file(FILE* stream);
void nocsim_grid_parse_router(char* def, ll_node* head);
void nocsim_grid_parse_PE(char* def, ll_node* head);
void nocsim_grid_parse_link(char* def, ll_node* head);
void nocsim_grid_parse_behavior(char* def, ll_node* head);

nocsim_node* nocsim_allocate_node(nocsim_node_type type, unsigned int row, unsigned int col, char* id);
char* nocsim_fmt_node(nocsim_node* node);
void nocsim_print_node(FILE* stream, nocsim_node* node);
void nocsim_dump_graphviz(FILE* stream, ll_node* head);
void nocsim_append_ll(ll_node* head, void* data);

void nocsim_behavior_DOR(nocsim_node* node);

void nocsim_step(ll_node* head, unsigned int tick);

#endif
