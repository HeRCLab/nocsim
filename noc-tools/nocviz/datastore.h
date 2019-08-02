#ifndef NOCVIZ_DATASTORE_H
#define NOCVIZ_DATASTORE_H

#include "../3rdparty/khash.h"
#include "../3rdparty/vec.h"
#include "operations.h"
#include "../common/util.h"

#include <tcl.h>

#include <stdio.h>


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
 * Because the formatted version of a value only changes when either the value
 * or format string changes, so the formatted version of each values is cached
 * and updated as needed.
 *
 * NOTE: the fmtcache is not guarnteed to be up to date, nocviz_ds_format()
 * should always be used when retrieving values for display.
 *
 * NOTE: nocviz_ds_free will free all keys and all values stored in the
 * datastore. While the setter functions will copy the keys used, any values
 * you insert MUST be heap-allocated, or nocviz_ds_free will segfault. You
 * should use nocviz_ds_del_*() to remove items you don't wish free-ed from
 * the datastore BEFORE calling nocviz_ds_free.
 *
 *****************************************************************************/

typedef vec_t(char*) strvec;

/* mapping of strings to other strings */
KHASH_MAP_INIT_STR(mstrstr, char*)

/* mapping of strings to vectors of strings */
KHASH_MAP_INIT_STR(mstrvec, strvec*)

/* maping of strings to operations */
KHASH_MAP_INIT_STR(mstrop, nocviz_op*)

typedef struct nocviz_ds_t {
	khash_t(mstrstr)* kvp;
	khash_t(mstrstr)* fmt;
	khash_t(mstrstr)* fmtcache;
	khash_t(mstrvec)* sections;
	khash_t(mstrop)* ops;
	Tcl_Interp* interp;
} nocviz_ds;

/* initialization */
nocviz_ds* nocviz_ds_init(void);

void nocviz_ds_free(nocviz_ds* ds);

/* getters */
char* nocviz_ds_get_kvp(nocviz_ds* ds, char* k);
char* nocviz_ds_get_fmt(nocviz_ds* ds, char* k);
char* nocviz_ds_get_fmtcache(nocviz_ds* ds, char* k);
nocviz_op* nocviz_ds_get_op(nocviz_ds* ds, char* opid);
strvec* nocviz_ds_get_section(nocviz_ds* ds, char* sect);

/* Retrieve a formatted value. If there is an error, then the unformatted value
 * will be used instead */
char* nocviz_ds_format(nocviz_ds* ds, char* k);

/* update fmtcache for a given key -- returns TCL_OK or TCL_ERROR */
int nocviz_ds_update_fmtcache(nocviz_ds*, char* k);

/* setters */
void nocviz_ds_set_kvp(nocviz_ds* ds, char* k, char* v);
void nocviz_ds_set_fmt(nocviz_ds* ds, char* k, char* fmt);
void nocviz_ds_set_fmtcache(nocviz_ds* ds, char* k, char* fmt);
void nocviz_ds_set_op(nocviz_ds* ds, char* opid, nocviz_op* oper);

strvec* nocviz_ds_new_section(nocviz_ds* ds, char* section_name);

/* deleters -- don't free values, just return them */
char* nocviz_ds_del_kvp(nocviz_ds* ds, char* k);
char* nocviz_ds_del_fmt(nocviz_ds* ds, char* k);
char* nocviz_ds_del_fmtcache(nocviz_ds* ds, char* k);
nocviz_op* nocviz_ds_del_op(nocviz_ds* ds, char* opid);
strvec* nocviz_ds_del_section(nocviz_ds* ds, char* section);

/* iterators */

#define nocviz_ds_foreach_kvp(ds, kvar, vvar, code) \
	kh_foreach(ds->kvp, kvar, vvar, code)
#define nocviz_ds_foreach_fmt(ds, kvar, vvar, code) \
	kh_foreach(ds->fmt, kvar, vvar, code)
#define nocviz_ds_foreach_fmtcache(ds, kvar, vvar, code) \
	kh_foreach(ds->fmtcache, kvar, vvar, code)
#define nocviz_ds_foreach_op(ds, kvar, vvar, code) \
	kh_foreach(ds->ops, kvar, vvar, code)
#define nocviz_ds_foreach_section(ds, kvar, vvar, code) \
	kh_foreach(ds->sections, kvar, vvar, code)


#endif
