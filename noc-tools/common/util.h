#ifndef NOCVIZ_UTIL_H
#define NOCVIZ_UTIL_H

#include <stdlib.h>
#include <err.h>

#define noctools_malloc(size) \
	__extension__ ({ \
		void* __res = malloc(size); \
		if (__res == NULL) { warn("malloc failed"); } \
		__res;})

#endif

/* "raw" debug printf */
#ifdef EBUG
#define dbrprintf(...) do { fprintf(stderr, __VA_ARGS__); } while (0)
#else
#define dbrprintf(...)
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

#define UNUSED(x) (void)(x)
