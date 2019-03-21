#ifndef NOCSIM_TYPES_H
#define NOCSIM_TYPES_H

#include <stdlib.h>

typedef enum nocsim_node_type_t {node_PE, node_router} nocsim_node_type;

#define NOCSIM_NODE_TYPE_TO_STR(typ) \
	(typ == node_PE) ? "PE" : \
	(typ == node_router) ? "router" : "ERROR"

/* Note that for convenience, we use this to subscript lists. The specific
 * order of values doesn't matter, but they need to be contiguous, and bounded
 * by 0...NOCSIM_NUM_LINKS */
typedef enum nocsim_direction_t {N=0, S=1, E=2, W=3, P=4} nocsim_direction;

#define NOCSIM_DIRECTION_TO_STR(dir) \
	(dir == N) ? "N" : \
	(dir == S) ? "S" : \
	(dir == E) ? "E" : \
	(dir == W) ? "W" : \
	(dir == P) ? "PE" : "ERROR"

/* Max number of links, which is 5 since there are 4 directions + PE. It
 * is important that this is eual to the number of types in nocsim_direction_t
 * */
#define NOCSIM_NUM_LINKS 5

/* Maximum FIFO size for PE outgoing FIFOs */
#define NOCSIM_FIFO_SIZE 128

typedef struct ll_node_t {
	struct ll_node_t* next;
	void* data;
} ll_node;

struct nocsim_node_t;
struct nocsim_link_t;

/* function pointer which we will call to perform routing for each node */
typedef void (*nocsim_behavior)(struct nocsim_node_t* node);

typedef struct nocsim_node_t {
	nocsim_node_type type;
	unsigned int row;
	unsigned int col;
	struct nocsim_link_t* incoming[NOCSIM_NUM_LINKS];
	struct nocsim_link_t* outgoing[NOCSIM_NUM_LINKS];
	char* id;
	ll_node* extra;

	/**** only used for PE type ******************************************/
	size_t fifo_size;
	ll_node* fifo_head;

	/**** only used for router type **************************************/
	nocsim_behavior behavior;

} nocsim_node;

typedef struct nocsim_packet_t {
	nocsim_node* from;
	nocsim_node* to;
	unsigned int age;
} nocsim_packet;

typedef struct nocsim_link_t {
	nocsim_node* from;
	nocsim_node* to;
	nocsim_packet* packet;
	nocsim_packet* packet_next;
} nocsim_link;

#endif
