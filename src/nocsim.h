#ifndef NOCSIM_H
#define NOCSIM_H

#ifndef __OpenBSD__
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

#include "nocsim_types.h"
#include "vec.h"
#include "khash.h"
#include "nocsim_constants.h"

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

/* heap-allocated new string by concatenating two existing strings. Will
 * break on non-null-terminated strings */
#define ezcat(str1, str2) \
	__extension__ ({ \
	char* buf = (char*) malloc(sizeof(char) * (strlen(str1) + strlen(str2) + 100)); \
	sprintf(buf, "%s%s", str1, str2); \
	buf;})

/* stack-allocated */
#define ezcats(str1, str2) \
	__extension__ ({ \
	char buf[sizeof(strlen(str1) + strlen(str2) + 100)]; \
	sprintf(buf, "%s%s", str1, str2); \
	buf;})

/* int to stack-allocated string */
#define i2sstr(n) __extension__ ({ \
	size_t __bufsz = snprintf(NULL, 0, "%i", n) * 2; \
	char __buf[__bufsz]; \
	snprintf(__buf, __bufsz, "%i", n); __buf;})

/* cast a string to a TCL string object (by value, s should be freed by called)
 * */
#define str2obj(s) Tcl_NewStringObj(s, strlen(s))

/* automatically allocated snprintf */
#define alloc_printf(fmt, ...) __extension__ ({ \
	size_t __ap_bufsz = snprintf(NULL, 0, fmt, __VA_ARGS__)*2; \
	char* __ap_buf; \
	alloc(__ap_bufsz, __ap_buf); \
	snprintf(__ap_buf, __ap_bufsz, fmt, __VA_ARGS__); \
	__ap_buf; })

#define Tcl_Evalf(interp, fmt, ...) __extension__ ({ \
	char* __evf_buf = alloc_printf(fmt, __VA_ARGS__); \
	int __evf_res = Tcl_Eval(interp, __evf_buf); \
	free(__evf_buf); __evf_res; })

#define conswritef(interp, fmt, ...) __extension__ ({ \
	char* __cw_buf = alloc_printf(fmt, __VA_ARGS__); \
	int __cw_res = Tcl_Evalf(interp, "conswrite {%s}", __cw_buf); \
	free(__cw_buf); __cw_res; })

#define errwritef(interp, fmt, ...) __extension__ ({ \
	char* __ew_buf = alloc_printf(fmt, __VA_ARGS__); \
	int __ew_res = Tcl_Evalf(interp, "errwrite {%s}", __ew_buf); \
	free(__ew_buf); __ew_res; })

/* add a key-value pair to a hash table of {str -> nocsim_node*}
 * h: hash table pointer
 * k: key (type: char*)
 * v: value (type: nocsim_node*) */
#define ez_kv_insert(h, k, v) __extension__ ({ \
	int status; \
	khint_t kiter; \
	/* insert key into hash table (no value assigned yet) */ \
	kiter = kh_put(nnptr, h, k, &status); \
	if (status == -1) { err(1, "Could not add %s (type: %d) at %d %d to hash table of nodes.", v->id, v->type, v->row, v->col); } \
	/* assign a value to the key */ \
	kh_value(h, kiter) = v;})


int main(int argc, char** argv);

void nocsim_grid_create_router(nocsim_state* state, char* id, unsigned int row, unsigned int col, char* behavior);
void nocsim_grid_create_PE(nocsim_state* state, char* id, unsigned int row, unsigned int col, char* behavior);
void nocsim_grid_create_link(nocsim_state* state, char* from_id, char* to_id);

void nocsim_init_node(nocsim_node* n, nocsim_node_type type, unsigned int row, unsigned int col, char* id);
char* nocsim_fmt_node(nocsim_node* node);
void nocsim_print_node(FILE* stream, nocsim_node* node);
void nocsim_dump_graphviz(FILE* stream, nocsim_state* state);
unsigned char with_P(float P);
unsigned int randrange(unsigned int lower, unsigned int upper);
void print_tcl_error(Tcl_Interp* interp);
char* get_tcl_library_path(void);
nocsim_node* nocsim_node_by_id(nocsim_state* state, char* id);
nocsim_link* nocsim_link_by_nodes(nocsim_state*, char* from, char* to);
#ifdef NOCSIM_GUI
void nocsim_console_writelines(AG_Console* console, const char* lines, AG_Color* c);
#endif

void nocsim_behavior_DOR(nocsim_node* node);
void nocsim_behavior_ADOR(nocsim_node* node);
void nocsim_DOR_one(nocsim_node* node, nocsim_flit* flit);

void nocsim_step(nocsim_state* state, Tcl_Interp* interp);
void nocsim_inject(nocsim_state* state, nocsim_node* from, nocsim_node* to);
void nocsim_handle_arrival(nocsim_state* state, nocsim_node* cursor, nocsim_direction dir);

#ifdef NOCSIM_GUI
nocsim_state* nocsim_create_interp(char* runme, AG_Console* cons, int argc, char** argv);
#else
nocsim_state* nocsim_create_interp(char* runme, int argc, char** argv);
#endif

#endif
