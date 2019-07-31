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
	);
	kh_destroy(mstrstr, ds->kvp);

	nocviz_ds_foreach_fmt(ds, key, str,
		free(str);
	);
	kh_destroy(mstrstr, ds->fmt);

	nocviz_ds_foreach_op(ds, key, oper,
		nocviz_op_free(oper);
	);
	kh_destroy(mstrop, ds->ops);

	nocviz_ds_foreach_section(ds, key, vec,
		vec_deinit(vec);
	);
	kh_destroy(mstrvec, ds->sections);

	free(ds);
}

char* nocviz_ds_get_kvp(nocviz_ds* ds, char* k) {
	khint_t iter;
	iter = kh_get(mstrstr, ds->kvp, k);
	if (iter == kh_end(ds->kvp)) { return NULL; }
	return kh_value(ds->kvp, iter);
}

char* nocviz_ds_get_fmt(nocviz_ds* ds, char* k) {
	khint_t iter;
	iter = kh_get(mstrstr, ds->fmt, k);
	if (iter == kh_end(ds->fmt)) { return NULL; }
	return kh_value(ds->fmt, iter);
}

char* nocviz_ds_get_fmtcache(nocviz_ds* ds, char* k) {
	khint_t iter;
	iter = kh_get(mstrstr, ds->fmtcache, k);
	if (iter == kh_end(ds->fmtcache)) { return NULL; }
	return kh_value(ds->fmtcache, iter);
}

nocviz_op* nocviz_ds_get_op(nocviz_ds* ds, char* k) {
	khint_t iter;
	iter = kh_get(mstrop, ds->ops, k);
	if (iter == kh_end(ds->ops)) { return NULL; }
	return kh_value(ds->ops, iter);
}

strvec* nocviz_ds_get_section(nocviz_ds* ds, char* k) {
	khint_t iter;
	iter = kh_get(mstrvec, ds->sections, k);
	if (iter == kh_end(ds->sections)) { return NULL; }
	return kh_value(ds->sections, iter);
}

char* nocviz_ds_format(nocviz_ds* ds, char* k) {
	return nocviz_ds_get_fmtcache(ds, k);
}

void nocviz_ds_set_kvp(nocviz_ds* ds, char* k, char* v) {
	int r;
	khint_t iter;
	iter = kh_put(mstrstr, ds->kvp, k, &r);
	kh_val(ds->kvp, iter) = v;
}

void nocviz_ds_set_fmt(nocviz_ds* ds, char* k, char* v) {
	int r;
	khint_t iter;
	iter = kh_put(mstrstr, ds->fmt, k, &r);
	kh_val(ds->fmt, iter) = v;
}

void nocviz_ds_set_fmtcache(nocviz_ds* ds, char* k, char* v) {
	int r;
	khint_t iter;
	iter = kh_put(mstrstr, ds->fmtcache, k, &r);
	kh_val(ds->fmtcache, iter) = v;
}

void nocviz_ds_set_op(nocviz_ds* ds, char* k, nocviz_op* oper) {
	int r;
	khint_t iter;
	iter = kh_put(mstrop, ds->ops, k, &r);
	kh_val(ds->ops, iter) = oper;
}

char* nocviz_ds_del_kvp(nocviz_ds* ds, char* k) {
	char* val;
	khint_t iter;
	iter = kh_get(mstrstr, ds->kvp, k);
	if (iter == kh_end(ds->kvp)) { return NULL; }
	val = kh_value(ds->kvp, iter);
	kh_del(mstrstr, ds->kvp, iter);
	return val;
}

char* nocviz_ds_del_fmt(nocviz_ds* ds, char* k) {
	char* val;
	khint_t iter;
	iter = kh_get(mstrstr, ds->fmt, k);
	if (iter == kh_end(ds->fmt)) { return NULL; }
	val = kh_value(ds->fmt, iter);
	kh_del(mstrstr, ds->fmt, iter);
	return val;
}

char* nocviz_ds_del_fmtcache(nocviz_ds* ds, char* k) {
	char* val;
	khint_t iter;
	iter = kh_get(mstrstr, ds->fmtcache, k);
	if (iter == kh_end(ds->fmtcache)) { return NULL; }
	val = kh_value(ds->fmtcache, iter);
	kh_del(mstrstr, ds->fmtcache, iter);
	return val;

}

nocviz_op* nocviz_ds_del_op(nocviz_ds* ds, char* opid) {
	nocviz_op* val;
	khint_t iter;
	iter = kh_get(mstrop, ds->ops, opid);
	if (iter == kh_end(ds->ops)) { return NULL; }
	val = kh_value(ds->ops, iter);
	kh_del(mstrop, ds->ops, iter);
	return val;
}

strvec* nocviz_ds_del_section(nocviz_ds* ds, char* section) {
	strvec* val;
	khint_t iter;
	iter = kh_get(mstrvec, ds->sections, section);
	if (iter == kh_end(ds->sections)) { return NULL; }
	val = kh_value(ds->sections, iter);
	kh_del(mstrvec, ds->sections, iter);
	return val;
}
