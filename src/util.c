#include "nocsim.h"

/**
 * @brief Allocate a new node.
 *
 * Does not populate any type-specific fields, other than to initialize them
 * to zero.
 *
 * @param type
 * @param row
 * @param col
 * @param id
 *
 * @return
 */
void nocsim_init_node(nocsim_node* n, nocsim_node_type type, unsigned int row, unsigned int col, char* id) {

	n->type = type;
	n->row = row;
	n->col = col;
	n->id = id;

	n->incoming[N] = NULL;
	n->incoming[W] = NULL;
	n->incoming[S] = NULL;
	n->incoming[E] = NULL;
	n->incoming[P] = NULL;
	n->outgoing[N] = NULL;
	n->outgoing[W] = NULL;
	n->outgoing[S] = NULL;
	n->outgoing[E] = NULL;
	n->outgoing[P] = NULL;

	n->P_inject = 0;
	n->behavior = NULL;

	n->node_number = 0;
	n->type_number = 0;

	n->injected = 0;
	n->routed = 0;
}

char* nocsim_fmt_node(nocsim_node* node) {
	char* s;

	if ((s = malloc((4*NOCSIM_GRID_LINELEN) * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	if (node == NULL) {
		snprintf(s, 4*NOCSIM_GRID_LINELEN,
				"node@0x%p:?,? 'unitialized'", (void*) node);

	} else {
		snprintf(s, 4*NOCSIM_GRID_LINELEN,
			"%s@0x%p:%u,%u '%s'",
			 NOCSIM_NODE_TYPE_TO_STR(node->type),
			 (void*) node, node->row, node->col, node->id);

	}

	return s;
}

void nocsim_print_node(FILE* stream, nocsim_node* node) {
	char* s;

	s = nocsim_fmt_node(node);
	fprintf(stream, "%s", s);
	free(s);
}

/**
 * @brief Dump a grid to graphviz on standard out.
 *
 * @param node
 */
void nocsim_dump_graphviz(FILE* stream, nocsim_state* state) {
	nocsim_node* cursor;
	unsigned int i;

	fprintf(stream, "digraph G {\n");

	vec_foreach(state->nodes, cursor, i) {
		/* XXX: this might be bad, unsigned int might be too short */
		fprintf(stream, "\"%p\" [label=\"%s\"]\n",
			(void*) cursor,
			cursor->id
		);
	}

	vec_foreach(state->nodes, cursor, i) {
		for (nocsim_direction d = N; d <= P; d++) {
			if (cursor->outgoing[d] == NULL) {
				continue;
			}

			fprintf(stream, "\"%p\" -> \"%p\"\n",
				(void*) cursor,
				(void*) cursor->outgoing[d]->to
			);
		}
	}

	fprintf(stream, "}\n");
}

/**
 * @brief Return 1 with probability P, and return 0 with probability 1-P.
 *
 * @param P
 *
 * @return
 */
unsigned char with_P(float P) {
	if ((P < 0) || (P > 1.0)) {
		err(1, "P=%f out of bounds", P);
	}

	if (rand() < (P * (1.0f * RAND_MAX))) {
		return 1;
	} else {
		return 0;
	}
}

unsigned int randrange(unsigned int lower, unsigned int upper) {
	return (((1.0f * rand()) / (1.0f * RAND_MAX)) * upper) + lower;
}

/* display an error traceback */
void print_tcl_error(Tcl_Interp* interp) {
	fprintf(stderr, "TCL error: %s\n", Tcl_GetStringResult(interp));
	fprintf(stderr, "$errorInfo is:\n%s\n", Tcl_GetVar(interp, "errorInfo", 0));
	Tcl_Eval(interp, "info errorstack");
	fprintf(stderr, "info errorstack is:\n%s\n", Tcl_GetStringResult(interp));
}

char* get_tcl_library_path(void) {
	FILE* fp;
	char* result = NULL;
	size_t n= 0;
	alloc(sizeof(char) * 512, result);
	fp = popen("echo 'puts $tcl_library' | tclsh", "r");
	while(getline(&result, &n, fp) != -1) {}
	fclose(fp);

	/* trim trailing \n */
	if (result[strnlen(result, 512)-1] == '\n') {
		result[strnlen(result, 512)-1] = '\0';
	}
	return result;
}

nocsim_node* nocsim_node_by_id(nocsim_state* state, char* id) {
	nocsim_node* cursor;
	unsigned int i;

	vec_foreach(state->nodes, cursor, i) {
		if (cursor->id == NULL) {
			err(1, "node@0x%p missing ID", (void*) cursor);
		}

		if (!strncmp(id, cursor->id, NOCSIM_GRID_LINELEN)) {
			return cursor;
		}
	}

	return NULL;

}

nocsim_link* nocsim_link_by_nodes(nocsim_state* state, char* from, char* to) {
	nocsim_node* from_node;
	nocsim_node* to_node;
	nocsim_direction d;

	from_node = nocsim_node_by_id(state, from);
	to_node = nocsim_node_by_id(state, to);

	if (from == NULL || to == NULL) { return NULL; }

	for (d = 0; d < P; d++) {
		if (from_node->outgoing[d] == NULL) { continue; }
		if (from_node->outgoing[d]->to == to_node) { return from_node->outgoing[d]; }
	}

	return NULL;

}
