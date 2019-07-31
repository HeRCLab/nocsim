/* test suite for datastore */

#include "../datastore.h"

int main() {
	nocviz_ds* ds;

	/* should be able to initialize and free without memory leaks */
	ds = nocviz_ds_init();
	nocviz_ds_free(ds);

	/* test that we can add a value into the main kvp and retrieve it later
	 * */
	ds = nocviz_ds_init();
	nocviz_ds_set_kvp(ds, "abc", "xyz");
	if (strcmp(nocviz_ds_get_kvp(ds, "abc"), "xyz") != 0) {
		return 1;
	}

	/* test adding a heap-allocated value should not memory leak */
	ds = nocviz_ds_init();
	char* val = malloc(128);
	sprintf(val, "%s", "test string!");
	nocviz_ds_set_kvp(ds, "abc", val);
	if (strcmp(nocviz_ds_get_kvp(ds, "abc"), "test string!") != 0) {
		return 2;
	}
	nocviz_ds_free(ds);

	/* test that we can overwrite a value in place */
	ds = nocviz_ds_init();
	nocviz_ds_set_kvp(ds, "abc", "xyz");
	nocviz_ds_set_kvp(ds, "abc", "123");
	if (strcmp(nocviz_ds_get_kvp(ds, "abc"), "123") != 0) {
		return 3;
	}
	nocviz_ds_free(ds);

	/* test addresses remain constant even after resize */
	ds = nocviz_ds_init();
	char* teststr = "TESTTEST";
	printf("%p\n", teststr);
	nocviz_ds_set_kvp(ds, "teststr", teststr);
	nocviz_ds_set_kvp(ds, "001", "dummy");
	nocviz_ds_set_kvp(ds, "002", "dummy");
	nocviz_ds_set_kvp(ds, "003", "dummy");
	nocviz_ds_set_kvp(ds, "004", "dummy");
	nocviz_ds_set_kvp(ds, "005", "dummy");
	nocviz_ds_set_kvp(ds, "006", "dummy");
	nocviz_ds_set_kvp(ds, "007", "dummy");
	nocviz_ds_set_kvp(ds, "008", "dummy");
	nocviz_ds_set_kvp(ds, "009", "dummy");
	nocviz_ds_set_kvp(ds, "010", "dummy");
	nocviz_ds_set_kvp(ds, "011", "dummy");
	nocviz_ds_set_kvp(ds, "012", "dummy");
	nocviz_ds_set_kvp(ds, "013", "dummy");
	nocviz_ds_set_kvp(ds, "014", "dummy");
	nocviz_ds_set_kvp(ds, "015", "dummy");
	nocviz_ds_set_kvp(ds, "016", "dummy");
	nocviz_ds_set_kvp(ds, "017", "dummy");
	nocviz_ds_set_kvp(ds, "018", "dummy");
	nocviz_ds_set_kvp(ds, "019", "dummy");
	nocviz_ds_set_kvp(ds, "020", "dummy");
	nocviz_ds_set_kvp(ds, "021", "dummy");
	nocviz_ds_set_kvp(ds, "022", "dummy");
	nocviz_ds_set_kvp(ds, "023", "dummy");
	nocviz_ds_set_kvp(ds, "024", "dummy");
	nocviz_ds_set_kvp(ds, "025", "dummy");
	nocviz_ds_set_kvp(ds, "026", "dummy");
	nocviz_ds_set_kvp(ds, "027", "dummy");
	nocviz_ds_set_kvp(ds, "028", "dummy");
	nocviz_ds_set_kvp(ds, "029", "dummy");
	nocviz_ds_set_kvp(ds, "030", "dummy");
	nocviz_ds_set_kvp(ds, "031", "dummy");
	nocviz_ds_set_kvp(ds, "032", "dummy");
	nocviz_ds_set_kvp(ds, "033", "dummy");
	nocviz_ds_set_kvp(ds, "034", "dummy");
	nocviz_ds_set_kvp(ds, "035", "dummy");
	nocviz_ds_set_kvp(ds, "036", "dummy");
	nocviz_ds_set_kvp(ds, "037", "dummy");
	nocviz_ds_set_kvp(ds, "038", "dummy");
	nocviz_ds_set_kvp(ds, "039", "dummy");
	nocviz_ds_set_kvp(ds, "040", "dummy");
	nocviz_ds_set_kvp(ds, "041", "dummy");
	nocviz_ds_set_kvp(ds, "042", "dummy");
	nocviz_ds_set_kvp(ds, "043", "dummy");
	nocviz_ds_set_kvp(ds, "044", "dummy");
	nocviz_ds_set_kvp(ds, "045", "dummy");
	nocviz_ds_set_kvp(ds, "046", "dummy");
	nocviz_ds_set_kvp(ds, "047", "dummy");
	nocviz_ds_set_kvp(ds, "048", "dummy");
	nocviz_ds_set_kvp(ds, "049", "dummy");
	nocviz_ds_set_kvp(ds, "050", "dummy");
	nocviz_ds_set_kvp(ds, "051", "dummy");
	nocviz_ds_set_kvp(ds, "052", "dummy");
	nocviz_ds_set_kvp(ds, "053", "dummy");
	nocviz_ds_set_kvp(ds, "054", "dummy");
	nocviz_ds_set_kvp(ds, "055", "dummy");
	nocviz_ds_set_kvp(ds, "056", "dummy");
	nocviz_ds_set_kvp(ds, "057", "dummy");
	nocviz_ds_set_kvp(ds, "058", "dummy");
	nocviz_ds_set_kvp(ds, "059", "dummy");
	nocviz_ds_set_kvp(ds, "060", "dummy");
	nocviz_ds_set_kvp(ds, "061", "dummy");
	nocviz_ds_set_kvp(ds, "062", "dummy");
	nocviz_ds_set_kvp(ds, "063", "dummy");
	nocviz_ds_set_kvp(ds, "064", "dummy");
	if (strcmp(nocviz_ds_get_kvp(ds, "teststr"), "TESTTEST") != 0) {
		return 4;
	}
	if (nocviz_ds_get_kvp(ds, "teststr") != teststr) {
		return 5;
	}
	nocviz_ds_free(ds);


	return 0;
}
