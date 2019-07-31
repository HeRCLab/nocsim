#ifndef NOCVIZ_DATASTORE_H
#define NOCVIZ_DATASTORE_H

#include "../3rdparty/khash.h"
#include "../3rdparty/vec.h"
#include "operations.h"

/******************************************************************************
 *
 * A datastore is used to provide they key-value pair store(s) attached to
 * nodes, links, as well as the general datastore. A datastore includes the
 * actual KVP store itself, a KVP store of for format strings, and a KVP store
 * for sections, associating section names with lists of keys that should
 * appear in them.
 *
 * A datatore also stores operations, and provides a key-value pair store
 * mapping operation IDs to operation struts.
 *
 *****************************************************************************/

/* mapping of strings to other strings */
KHASH_MAP_INIT_STR(mstrstr, char*);

/* mapping of strings to vectors of strings */
KHASH_MAP_INIT_STR(mstrvec, vec_t(char*)*);

/* maping of strings to operations */
KHASH_MAP_INIT_STR(mstrop, nocviz_op*);

typedef struct nocviz_ds_t {
	khash_t(mstrstr)* kvp;
	khash_t(mstrstr)* fmt;
	khash_t(mstrvec)* sections;
	khash_t(mstrop)* ops;
} nocviz_ds;

/* initialization */
nocviz_datastore* nocviz_ds_init(void);

/* getters */
char* nocviz_ds_get_kvp(nocviz_ds* ds, char* k);
char* nocviz_ds_get_fmt(nocviz_ds* ds, char* k);
nocviz_op* nocviz_ds_get_op(nocviz_ds* ds, char* opid);
vec_t(char*)* nocviz_ds_get_section(nocviz_ds* ds, char* sect);

/* retrieve a formatted value */
char* nocviz_ds_format(nocviz_ds* ds, char* k);

/* setters */
void nocviz_ds_set_kvp(nocviz_ds* ds, char* k, char* v);
void nocviz_ds_set_fmt(nocviz_ds* ds, char* k, char* fmt);
void nocviz_ds_set_op(nocviz_ds* ds, char* opid, nocsim_op* oper);
/* no setter for sections, caller can modify the vec_t directly */

/* deleters -- don't free values, just return them */
char* nocviz_ds_del_kvp(nocviz_ds* ds, char* k);
char* nocviz_ds_del_fmt(nocviz_ds* ds, char* k);
nocviz_op* nocviz_ds_del_op(nocviz_ds* ds, char* opid);
vec_t(char*)* nocviz_ds_del_section(nocviz_ds* ds, char* section);

/* deinitialization */
nocviz_ds_free(nocviz_ds* ds);

/* iterators */
/* TODO

#define nocviz_ds_foreach_kvp(ds, kvar, vvar, code)
#define nocviz_ds_foreach_fmt(ds, kvar, vvar, code)
#define nocviz_ds_foreach_op(ds, kvar, vvar, code)
#define nocviz_ds_foreach_section(ds, kvar, vvar, code)

*/

#endif
