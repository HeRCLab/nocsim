/* test suite for datastore */

#include "../datastore.h"
#include "test_util.h"
#include "../../common/util.h"
#include "../../3rdparty/vec.h"

int main() {
	nocviz_ds* ds;

	/* should be able to initialize and free without memory leaks */
	ds = nocviz_ds_init();
	nocviz_ds_free(ds);

	/* test adding a heap-allocated value should not memory leak */
	ds = nocviz_ds_init();
	char* val = malloc(128);
	sprintf(val, "%s", "test string!");
	nocviz_ds_set_kvp(ds, "abc", val);
	str_should_equal(nocviz_ds_get_kvp(ds, "abc"), "test string!");

	nocviz_ds_set_fmt(ds, "abc", strdup("aaa"));
	str_should_equal(nocviz_ds_get_fmt(ds, "abc"), "aaa");

	nocviz_ds_set_fmtcache(ds, "abc", strdup("bbb"));
	str_should_equal(nocviz_ds_get_fmtcache(ds, "abc"), "bbb");

	nocviz_op* op = nocviz_op_init("123", "456");
	nocviz_ds_set_op(ds, "abc", op);
	str_should_equal(nocviz_ds_get_op(ds, "abc")->description, "456");
	str_should_equal(nocviz_ds_get_op(ds, "abc")->script, "123");

	strvec* sect = nocviz_ds_new_section(ds, "xyz");
	vec_push(sect, "123456");
	str_should_equal(nocviz_ds_get_section(ds, "xyz")->data[0], "123456");

	nocviz_ds_free(ds);

	/* test that we can overwrite a value in place */
	ds = nocviz_ds_init();
	nocviz_ds_set_kvp(ds, "abc", strdup("xyz"));
	nocviz_ds_set_kvp(ds, "abc", strdup("123"));
	nocviz_ds_set_kvp(ds, "abc", strdup("456"));
	nocviz_ds_set_kvp(ds, "abc", strdup("789"));
	str_should_equal(nocviz_ds_get_kvp(ds, "abc"), "789");
	nocviz_ds_free(ds);

	/* test addresses remain constant even after resize */
	ds = nocviz_ds_init();
	char* teststr = strdup("TESTTEST");
	nocviz_ds_set_kvp(ds, "teststr", teststr);
	nocviz_ds_set_kvp(ds, "001", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "002", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "003", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "004", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "005", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "006", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "007", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "008", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "009", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "010", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "011", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "012", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "013", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "014", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "015", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "016", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "017", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "018", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "019", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "020", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "021", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "022", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "023", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "024", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "025", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "026", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "027", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "028", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "029", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "030", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "031", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "032", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "033", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "034", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "035", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "036", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "037", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "038", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "039", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "040", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "041", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "042", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "043", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "044", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "045", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "046", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "047", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "048", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "049", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "050", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "051", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "052", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "053", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "054", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "055", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "056", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "057", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "058", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "059", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "060", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "061", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "062", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "063", strdup("dummy"));
	nocviz_ds_set_kvp(ds, "064", strdup("dummy"));
	str_should_equal(nocviz_ds_get_kvp(ds, "teststr"), "TESTTEST");
	should_be_true(nocviz_ds_get_kvp(ds, "teststr") == teststr);
	nocviz_ds_free(ds);

	ds = nocviz_ds_init();
	nocviz_ds_set_kvp(ds, "key1", strdup("123.45678"));
	str_should_equal(nocviz_ds_format(ds, "key1"), "123.45678");
	nocviz_ds_set_fmt(ds, "key1", strdup("%2.3f"));
	str_should_equal(nocviz_ds_format(ds, "key1"), "123.457");

	/* default formatting should be %s */
	nocviz_ds_set_kvp(ds, "key2", strdup("abcd"));
	str_should_equal(nocviz_ds_format(ds, "key2"), "abcd");

	/* undefined keys should use a predictable error message */
	str_should_equal(nocviz_ds_format(ds, "key3"), "FORMAT ERROR");
	nocviz_ds_free(ds);

	return 0;
}
