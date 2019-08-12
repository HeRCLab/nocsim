#include "nocsim.h"

/* this is required to ensure the namespace and backing datastore is properly
 * cleaned up when the interpreter exits */
void nocsim_namespace_delete(ClientData cdata) {
	nocsim_state* s = cdata;
	nocsim_link* l;
	nocsim_node* n;
	unsigned int i;
	unsigned int j;
	nocsim_flit* f;

	dbprintf("deallocating nocsim namespace\n");

	/* destroy all links */
	vec_foreach(s->links, l, i) {
		if (l->flit != NULL) {
			free(l->flit);
		}
		free(l);
	}

	/* destroy all nodes */
	vec_foreach(s->nodes, n, i) {
		if (n->type == node_PE) {
			vec_foreach(n->pending, f, j) {
				free(f);
			}
			vec_deinit(n->pending);
			free(n->pending);
		}
		free(n);
	}

	/* free node map */
	kh_destroy(nnptr, s->node_map);

	/* free node list */
	vec_deinit(s->nodes);
	free(s->nodes);

	/* free link list */
	vec_deinit(s->links);
	free(s->links);

	/* free error string */
	if (s->errstr != NULL) {
		free(s->errstr);
	}

	free(s);
}

int DLLEXPORT
Nocsim_Init(Tcl_Interp *interp)
{
	Tcl_Namespace *nsPtr; /* pointer to hold our own new namespace */

	nocsim_state* state;
	alloc(sizeof(nocsim_state), state);

	if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL) {
		return TCL_ERROR;
	}

	nsPtr = Tcl_CreateNamespace(interp, "nocsim", state, nocsim_namespace_delete);
	if (nsPtr == NULL) {
		return TCL_ERROR;
	}
	nocsim_create_state(interp, state);

	Tcl_PkgProvide(interp, "nocsim", NOC_TOOLS_VERSION);
	return TCL_OK;
}
