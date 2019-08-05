#ifndef NOCVIZ_UTIL_H
#define NOCVIZ_UTIL_H

#include <stdlib.h>
#include <err.h>

/*** GENERAL UTILITIES *******************************************************/

#define noctools_malloc(size) \
	__extension__ ({ \
		void* __res = malloc(size); \
		if (__res == NULL) { warn("malloc failed"); } \
		__res;})

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

#define string_equals(s1, s2) (strcmp(s1, s2) == 0)

/*** TCL UTILITIES ***********************************************************/

#define Tcl_RequireArgs(interp, n, msg) if (objc != n) { \
	Tcl_WrongNumArgs(interp, 0, objv, msg); return TCL_ERROR; }

#define Tcl_ObjToInt(interp, obj, ptr) if (Tcl_GetIntFromObj(interp, obj, ptr) != TCL_OK) \
			{return TCL_ERROR;}

#define Tcl_SetResultf(interp, fmt, ...) do { \
		char* __msg; \
		int __r; \
		__r = asprintf(&__msg, fmt, __VA_ARGS__); \
		if (__r < 0) { \
			warn("asprintf(\"%p, %s, %s\") failed", (void*) &__msg, fmt, #__VA_ARGS__); \
			break; \
		} \
		Tcl_SetObjResult(interp, Tcl_NewStringObj(__msg, strlen(__msg))); \
		free(__msg); \
	} while(0)

#define Tcl_Evalf(interp, fmt, ...) __extension__ ({ \
	char* __evf_buf; \
	if (asprintf(&__evf_buf, fmt, __VA_ARGS__) < 0) { \
		warn("asprintf(\"%p, %s, %s\") failed", (void*) &__evf_buf, fmt, #__VA_ARGS__); \
	} \
	int __evf_res = Tcl_Eval(interp, __evf_buf); \
	free(__evf_buf); __evf_res; })

#endif /* NOCVIZ_UTIL_H */
