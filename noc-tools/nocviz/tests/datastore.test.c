/* test suite for datastore */

#include "../datastore.h"

int main() {
	nocviz_ds* ds;

	/* should be able to initialize and free without memory leaks */
	ds = nocviz_ds_init();
	nocviz_ds_free(ds);

	return 0;
}
