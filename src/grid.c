#include "nocsim.h"

/* This file contains methods relating to the "grid" of routers, PEs, and links
 * */

/**
 * @brief Parse an entire file containing a grid definition
 *
 * @param stream
 *
 * @return linked list of all nodes in the grid
 */
ll_node* nocsim_grid_parse_file(FILE* stream) {
	char* token;
	char* rest;
	ll_node* head;

	if ((head = malloc(sizeof(ll_node))) == NULL) {
		err(1, NULL);
	}

	head->data = NULL;
	head->next = NULL;

	if ((token = malloc(NOCSIM_GRID_LINELEN * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	if ((rest = malloc(NOCSIM_GRID_LINELEN * sizeof(char))) == NULL) {
		err(1, NULL);
	}

	*token = 0; *rest = 0;

	while(fscanf(stream, "%s %s\n", token, rest) != EOF) {
		dprintf("token='%s' rest='%s'\n", token, rest);
	
		if (!strncmp(token, "router", NOCSIM_GRID_LINELEN)) {
		} else {
			err(1, "syntax error in grid definition, unknown token '%s'", token);
		}

		*token = 0; *rest = 0;
	}

	dprintf("reached end of grid definition file\n");
}

/**
 * @brief Parse a definition of a router and create an appropriate node for it
 *
 * @param def
 */
void nocsim_grid_parse_router(char* def, ll_node* head) {

}
