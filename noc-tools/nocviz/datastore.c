#include "datastore.h"

nocviz_ds* nocviz_ds_init(void) {
	nocviz_ds* ds;

	ds = noctools_malloc(sizeof(nocviz_ds));

	ds->kvp = kh_init(mstrstr);
	ds->fmt = kh_init(mstrstr);
	ds->sections = kh_init(mstrvec);
	ds->ops = kh_init(mstrop);

	return ds;
}

void nocviz_ds_free(nocviz_ds* ds) {
	const char* key __attribute__((unused));
	char* str;
	strvec* vec;
	nocviz_op* oper;

	nocviz_ds_foreach_kvp(ds, key, str,
		free(str);

		/* Unsafe cast of const char* to char*, since we can't free
		 * it otherwise. This could cause problems if we tried to
		 * access the table other than to destroy it later. */
		free((char*) key);
	);
	kh_destroy(mstrstr, ds->kvp);

	nocviz_ds_foreach_fmt(ds, key, str,
		free(str);
		free((char*) key);
	);
	kh_destroy(mstrstr, ds->fmt);

	nocviz_ds_foreach_op(ds, key, oper,
		nocviz_op_free(oper);
		free((char*) key);
	);
	kh_destroy(mstrop, ds->ops);

	nocviz_ds_foreach_section(ds, key, vec,
		vec_deinit(vec);
		free((char*) key);
	);
	kh_destroy(mstrvec, ds->sections);

	free(ds);
}

#define getter_logic(__ds, __k, __typ, __memb) \
	do { \
	khint_t __iter; \
	__iter = kh_get(__typ, __ds->__memb, __k); \
	if (__iter == kh_end(__ds->__memb)) { return NULL; } \
	return kh_value(__ds->__memb, __iter); \
	} while(0);

char* nocviz_ds_get_kvp(nocviz_ds* ds, char* k) {
	getter_logic(ds, k, mstrstr, kvp);
}

char* nocviz_ds_get_fmt(nocviz_ds* ds, char* k) {
	getter_logic(ds, k, mstrstr, fmt);
}

char* nocviz_ds_get_fmtcache(nocviz_ds* ds, char* k) {
	getter_logic(ds, k, mstrstr, fmtcache);
}

nocviz_op* nocviz_ds_get_op(nocviz_ds* ds, char* k) {
	getter_logic(ds, k, mstrop, ops);
}

strvec* nocviz_ds_get_section(nocviz_ds* ds, char* k) {
	getter_logic(ds, k, mstrvec, sections);
}

#undef getter_logic

char* nocviz_ds_format(nocviz_ds* ds, char* k) {
	return nocviz_ds_get_fmtcache(ds, k);
}

#define setter_logic(__ds, __k, __v, __typ, __memb, __free, __del) do { \
		int __r; khint_t __iter; \
		__iter = kh_get(__typ, __ds->__memb, __k); \
		if (__iter != kh_end(__ds->__memb)) { /* key not present */ \
			/* value already exists, needs to be freed */ \
			__free(__del(__ds, __k)); \
		} \
		__iter = kh_put(__typ, __ds->__memb, strdup(__k), &__r); \
		kh_val(__ds->__memb, __iter) = __v; \
	} while(0)

void nocviz_ds_set_kvp(nocviz_ds* ds, char* k, char* v) {
	setter_logic(ds, k, v, mstrstr, kvp, free, nocviz_ds_del_kvp);
}

void nocviz_ds_set_fmt(nocviz_ds* ds, char* k, char* v) {
	setter_logic(ds, k, v, mstrstr, fmt, free, nocviz_ds_del_fmt);
}

void nocviz_ds_set_fmtcache(nocviz_ds* ds, char* k, char* v) {
	setter_logic(ds, k, v, mstrstr, fmtcache, free, nocviz_ds_del_fmtcache);
}

void nocviz_ds_set_op(nocviz_ds* ds, char* k, nocviz_op* oper) {
	setter_logic(ds, k, oper, mstrop, ops, nocviz_op_free, nocviz_ds_del_op);
}

#undef setter_logic

#define del_logic(__ds, __k, __typ, __memb, __rettyp) do { \
	__rettyp __val; \
	khint_t __iter; \
	__iter = kh_get(__typ, __ds->__memb, __k); \
	if (__iter == kh_end(__ds->__memb)) { return NULL; } \
	__val = kh_value(__ds->__memb, __iter); \
	free((char*) kh_key(__ds->__memb, __iter)); \
	kh_del(__typ, __ds->__memb, __iter); \
	return __val; } while (0)

char* nocviz_ds_del_kvp(nocviz_ds* ds, char* k) {
	del_logic(ds, k, mstrstr, kvp, char*);
}

char* nocviz_ds_del_fmt(nocviz_ds* ds, char* k) {
	del_logic(ds, k, mstrstr, fmt, char*);
}

char* nocviz_ds_del_fmtcache(nocviz_ds* ds, char* k) {
	del_logic(ds, k, mstrstr, fmtcache, char*);
}

nocviz_op* nocviz_ds_del_op(nocviz_ds* ds, char* opid) {
	del_logic(ds, opid, mstrop, ops, nocviz_op*);
}

strvec* nocviz_ds_del_section(nocviz_ds* ds, char* section) {
	del_logic(ds, section, mstrvec, sections, strvec*);
}