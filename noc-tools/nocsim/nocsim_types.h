#ifndef NOCSIM_TYPES_H
#define NOCSIM_TYPES_H

#include <stdlib.h>
#include <tcl.h>

#ifdef NOCSIM_GUI
#include <agar/core.h>
#include <agar/gui.h>
#endif

#include "../3rdparty/khash.h"
#include "../3rdparty/vec.h"

typedef enum nocsim_node_type_t {node_PE, node_router, type_undefined} nocsim_node_type;

#define NOCSIM_NODE_TYPE_TO_STR(typ) \
	(typ == node_PE) ? "PE" : \
	(typ == node_router) ? "router" : "ERROR"

#define NOCSIM_STR_TO_NODE_TYPE(s)  __extension__ ({ \
	nocsim_node_type t = -1; \
	if      (!strncasecmp(s, "router",  32)) { t = node_router; } \
	else if (!strncasecmp(s, "pe",  32)) { t = node_PE; } \
	else                                { t = type_undefined; } \
	t;})

/* Note that for convenience, we use this to subscript lists. The specific
 * order of values doesn't matter, but they need to be contiguous, and bounded
 * by 0...NOCSIM_NUM_LINKS */
typedef enum nocsim_direction_t {N=0, S=1, E=2, W=3, P=4, DIR_UNDEF=5} nocsim_direction;

#define NOCSIM_DIRECTION_TO_STR(dir) \
	(dir == N) ? "N" : \
	(dir == S) ? "S" : \
	(dir == E) ? "E" : \
	(dir == W) ? "W" : \
	(dir == DIR_UNDEF) ? "UNDEFINED" : \
	(dir == P) ? "PE" : "ERROR"

#define NOCSIM_DIRECTION_VALID(dir) \
	( (dir == N) || (dir == S) || (dir == E) || (dir == W) || (dir == P) )

#define NOCSIM_STR_TO_DIRECTION(s) __extension__ ({ \
	nocsim_direction d = -1; \
	if      (!strncasecmp(s, "N",  32     )  ) { d = N; } \
	else if (!strncasecmp(s, "up",  32    )  ) { d = N; } \
	else if (!strncasecmp(s, "north",  32 )  ) { d = N; } \
	else if (!strncasecmp(s, "S",  32     )  ) { d = S; } \
	else if (!strncasecmp(s, "down",  32  )  ) { d = S; } \
	else if (!strncasecmp(s, "south",  32 )  ) { d = S; } \
	else if (!strncasecmp(s, "E",  32     )  ) { d = E; } \
	else if (!strncasecmp(s, "right",  32 )  ) { d = E; } \
	else if (!strncasecmp(s, "east",  32  )  ) { d = E; } \
	else if (!strncasecmp(s, "W",  32     )  ) { d = W; } \
	else if (!strncasecmp(s, "left",  32  )  ) { d = W; } \
	else if (!strncasecmp(s, "west",  32  )  ) { d = W; } \
	else if (!strncasecmp(s, "PE", 32     )  ) { d = P; } \
	else if (!strncasecmp(s, "P", 32      )  ) { d = P; } \
	else                                { d = DIR_UNDEF; } \
	d;})

typedef enum nocsim_instrument_t {
	INSTRUMENT_UNDEFINED = 0,
	INSTRUMENT_SPAWN,
	INSTRUMENT_ROUTE,
	INSTRUMENT_ARRIVE,
	INSTRUMENT_BACKROUTE,
	INSTRUMENT_TICK,
	INSTRUMENT_INJECT,
	INSTRUMENT_DEQUEUE,
	ENUMSIZE_INSTRUMENT
} nocsim_instrument;

#define NOCSIM_INSTRUMENT_TO_STR(ins) \
	(ins == INSTRUMENT_SPAWN) ? "spawn" : \
	(ins == INSTRUMENT_ROUTE) ? "route" : \
	(ins == INSTRUMENT_ARRIVE) ? "arrive" : \
	(ins == INSTRUMENT_BACKROUTE) ? "backroute" : \
	(ins == INSTRUMENT_INJECT) ? "inject" : \
	(ins == INSTRUMENT_DEQUEUE) ? "dequeue" : \
	(ins == INSTRUMENT_TICK) ? "tick" : "INSTRUMENT UNDEFINED"

#define NOCSIM_STR_TO_INSTRUMENT(s) \
	(!strncasecmp(s, "spawn", 32)) ? INSTRUMENT_SPAWN: \
	(!strncasecmp(s, "route", 32)) ? INSTRUMENT_ROUTE : \
	(!strncasecmp(s, "arrive", 32)) ? INSTRUMENT_ARRIVE : \
	(!strncasecmp(s, "inject", 32)) ? INSTRUMENT_INJECT: \
	(!strncasecmp(s, "backroute", 32)) ? INSTRUMENT_BACKROUTE : \
	(!strncasecmp(s, "tick", 32)) ? INSTRUMENT_TICK : \
	(!strncasecmp(s, "dequeue", 32)) ? INSTRUMENT_DEQUEUE: \
	INSTRUMENT_UNDEFINED

typedef enum nocsim_result_t {
	NOCSIM_RESULT_OK,
	NOCSIM_RESULT_ERROR,
} nocsim_result;

#define NOCSIM_RESULT_TO_STR(res) \
	(res == NOCSIM_RESULT_OK) ? "ok" : \
	(res == NOCSIM_RESULT_ERROR) ? "general error" : \
	"unknown error"

/* Max number of links, which is 5 since there are 4 directions + PE. It
 * is important that this is eual to the number of types in nocsim_direction_t
 * */
#define NOCSIM_NUM_LINKS 5

/* Maximum FIFO size for PE outgoing FIFOs */
#define NOCSIM_FIFO_SIZE 128

struct nocsim_node_t;
struct nocsim_link_t;
struct nocsim_flit_t;

typedef vec_t(struct nocsim_flit_t*) flitlist;

/* function pointer which we will call to perform routing for each node */
typedef void (*nocsim_behavior)(struct nocsim_node_t* node);

typedef struct nocsim_node_t {
	nocsim_node_type type;
	unsigned int row;
	unsigned int col;
	struct nocsim_link_t* incoming[NOCSIM_NUM_LINKS];
	struct nocsim_link_t* outgoing[NOCSIM_NUM_LINKS];
	char* id;
	unsigned int node_number;
	unsigned int type_number;
	long routed;
	long backrouted;

	/* used to define either routing behavior or injection behavior
	 * according to node type */
	char* behavior;

	/**** only used for PE type ******************************************/
	flitlist* pending;
	float P_inject;
	long spawned;
	long dequeued;
	long injected;
	long arrived;

} nocsim_node;

typedef vec_t(nocsim_node*) nodelist;
typedef vec_t(struct nocsim_link_t*) linklist;

typedef struct nocsim_flit_t{
	nocsim_node* from;
	nocsim_node* to;
	unsigned long spawned_at;
	unsigned long injected_at;
	unsigned long hops;
	unsigned long flit_no;
} nocsim_flit;

typedef struct nocsim_link_t {
	nocsim_node* from;
	nocsim_node* to;
	nocsim_flit* flit;
	nocsim_flit* flit_next;
	long load;
} nocsim_link;

/* hash table struct name will have "kh_nnptr" in it */
KHASH_MAP_INIT_STR(nnptr, nocsim_node*)
typedef khash_t(nnptr) nodemap;

typedef struct nocsim_state_t {

	/* in cases where we don't want to simulate anything, this is asserted
	 * -- this is an implementation detail of how flags such as -g are
	 *  implemented */
	unsigned char enable_simulation;

	unsigned int RNG_seed;
	unsigned int num_PE;
	unsigned int num_router;
	unsigned int num_node;
	unsigned long flit_no;
	unsigned long tick;
	float default_P_inject;
	unsigned int max_ticks;
	char* title;
	nodelist* nodes;
	linklist* links;
	/* used for quick node lookups by ID */
	nodemap* node_map;
	unsigned int max_row;
	unsigned int max_col;
	long spawned;
	long injected;
	long dequeued;
	long backrouted;
	long routed;
	long arrived;
	
	/* used by some functions to return an error string */
	char* errstr;

	/* used during behavior callbacks */
	nocsim_node* current;

	Tcl_Interp* interp;

	char* instruments[(int) ENUMSIZE_INSTRUMENT];

} nocsim_state;

#endif
