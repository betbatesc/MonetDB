/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.  If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 1997 - July 2008 CWI, August 2008 - 2021 MonetDB B.V.
 */

#include "monetdb_config.h"
#include "sql_catalog.h"
#include "sql_storage.h"

const char *TID = "%TID%";

int
base_key( sql_base *b )
{
	return hash_key(b->name);
}

static void *
_list_find_name(list *l, const char *name)
{
	node *n;

	if (l) {
		MT_lock_set(&l->ht_lock);
		if ((!l->ht || l->ht->size*16 < list_length(l)) && list_length(l) > HASH_MIN_SIZE && l->sa) {
			l->ht = hash_new(l->sa, list_length(l), (fkeyvalue)&base_key);
			if (l->ht == NULL) {
				MT_lock_unset(&l->ht_lock);
				return NULL;
			}

			for (n = l->h; n; n = n->next ) {
				sql_base *b = n->data;
				int key = base_key(b);

				if (hash_add(l->ht, key, b) == NULL) {
					MT_lock_unset(&l->ht_lock);
					return NULL;
				}
			}
		}
		if (l->ht) {
			int key = hash_key(name);
			sql_hash_e *he = l->ht->buckets[key&(l->ht->size-1)];

			for (; he; he = he->chain) {
				sql_base *b = he->value;

				if (b->name && strcmp(b->name, name) == 0 && !b->deleted) {
					MT_lock_unset(&l->ht_lock);
					return b;
				}
			}
			MT_lock_unset(&l->ht_lock);
			return NULL;
		}
		MT_lock_unset(&l->ht_lock);
		for (n = l->h; n; n = n->next) {
			sql_base *b = n->data;

			/* check if names match */
			if (name[0] == b->name[0] && strcmp(name, b->name) == 0 && !b->deleted) {
				return b;
			}
		}
	}
	return NULL;
}

static sql_base *
tr_get_object(sql_trans *tr, sql_base *b)
{
	while (b) {
		if (b->ts == tr->tid || b->ts < tr->ts)
			return b;
		else
			b = b->older;
	}
	return b;
}

int
tr_empty(sql_trans *tr, list *l)
{
	if (list_empty(l))
		return 1;
	for(node *n = l->h; n; n=n->next) {
		sql_base *b = tr_get_object(tr, n->data);

		if (b && !b->deleted)
			return 0;
	}
	return 1;
}

void
trans_add(sql_trans *tr, sql_base *b, void *data, tc_cleanup_fptr cleanup, tc_log_fptr log)
{
	sql_change *change = SA_ZNEW(tr->sa, sql_change);
	change->obj = b;
	change->data = data;
	change->cleanup = cleanup;
	change->log = log;
	tr->changes = sa_list_append(tr->sa, tr->changes, change);
}

int
tr_version_of_parent(sql_trans *tr, ulng ts)
{
	for( tr = tr->parent; tr; tr = tr->parent)
		if (tr->tid == ts)
			return 1;
	return 0;
}

static void *
tr_find_base(sql_trans *tr, sql_base *b)
{
	while(b && b->ts != tr->tid && (tr->parent && !tr_version_of_parent(tr, b->ts)) && b->ts > tr->ts)
			b = b->older;
	return b;
}

static void *
tr_find_name(sql_trans *tr, changeset * cs, const char *name)
{
	sql_base *b =  _list_find_name(cs->set, name);
	return tr_find_base(tr, b);
}

static void *
_cs_find_name(changeset * cs, const char *name)
{
	return _list_find_name(cs->set, name);
}

node *
cs_find_name(changeset * cs, const char *name)
{
	return list_find_name(cs->set, name);
}

node *
list_find_name(list *l, const char *name)
{
	node *n;

	if (l)
		for (n = l->h; n; n = n->next) {
			sql_base *b = n->data;

			/* check if names match */
			if (name[0] == b->name[0] && strcmp(name, b->name) == 0 && !b->deleted) {
				return n;
			}
		}
	return NULL;
}

node *
cs_find_id(changeset * cs, sqlid id)
{
	node *n;
	list *l = cs->set;

	if (l)
		for (n = l->h; n; n = n->next) {
			sql_base *b = n->data;

			/* check if names match */
			if (b->id == id) {
				return n;
			}
		}
	return NULL;
}

node *
list_find_id(list *l, sqlid id)
{
	if (l) {
		node *n;
		for (n = l->h; n; n = n->next) {

			/* check if ids match */
			if (id == *(sqlid *) n->data) {
				return n;
			}
		}
	}
	return NULL;
}

node *
list_find_base_id(list *l, sqlid id)
{
	if (l) {
		node *n;
		for (n = l->h; n; n = n->next) {
			sql_base *b = n->data;

			if (id == b->id)
				return n;
		}
	}
	return NULL;
}

sql_key *
find_sql_key(sql_table *t, const char *kname)
{
	return _cs_find_name(&t->keys, kname);
}

sql_key *
sql_trans_find_key(sql_trans *tr, sqlid id)
{
	if (tr->cat->schemas.set) {
		for (node *n = tr->cat->schemas.set->h; n; n = n->next) {
			sql_schema *s = n->data;
			sql_base *b = os_find_id(s->keys, tr, id);
			if (b)
				return (sql_key*)b;
		}
	}
	return NULL;
}

sql_idx *
find_sql_idx(sql_table *t, const char *iname)
{
	return _cs_find_name(&t->idxs, iname);
}

sql_idx *
sql_trans_find_idx(sql_trans *tr, sqlid id)
{
	if (tr->cat->schemas.set) {
		for (node *n = tr->cat->schemas.set->h; n; n = n->next) {
			sql_schema *s = n->data;
			sql_base *b = os_find_id(s->idxs, tr, id);
			if (b)
				return (sql_idx*)b;
		}
	}
	return NULL;
}

sql_column *
find_sql_column(sql_table *t, const char *cname)
{
	return _cs_find_name(&t->columns, cname);
}

sql_part *
find_sql_part_id(sql_trans *tr, sql_table *t, sqlid id)
{
	node *n = cs_find_id(&t->members, id);

	if (n)
		return tr_find_base(tr, n->data);
	return NULL;
}

sql_table *
find_sql_table(sql_trans *tr, sql_schema *s, const char *tname)
{
	return (sql_table*)os_find_name(s->tables, tr, tname);
}

sql_table *
find_sql_table_id(sql_trans *tr, sql_schema *s, sqlid id)
{
	return (sql_table*)os_find_id(s->tables, tr, id);
}

sql_table *
sql_trans_find_table(sql_trans *tr, sqlid id)
{
	node *n;
	sql_table *t = NULL;

	if (tr->cat->schemas.set) {
		for (n = tr->cat->schemas.set->h; n && !t; n = n->next) {
			sql_schema *s = n->data;
			sql_base *m = os_find_id(s->tables, tr, id);
			if (m)
				t = (sql_table*)m;
		}
	}
	return t;
}

sql_sequence *
find_sql_sequence(sql_trans *tr, sql_schema *s, const char *sname)
{
	return (sql_sequence*)os_find_name(s->seqs, tr, sname);
}

sql_schema *
find_sql_schema(sql_trans *tr, const char *sname)
{
	if (tr->tmp && strcmp(sname, "tmp")==0)
		return tr->tmp;
	return tr_find_name(tr, &tr->cat->schemas, sname);
}

sql_schema *
find_sql_schema_id(sql_trans *tr, sqlid id)
{
	if (tr->tmp && tr->tmp->base.id == id)
		return tr->tmp;
	node *n = cs_find_id(&tr->cat->schemas, id);

	if (n)
		return n->data;
	return NULL;
}

node *
find_sql_schema_node(sql_trans *tr, sqlid id)
{
	return cs_find_id(&tr->cat->schemas, id);
}

sql_type *
find_sql_type(sql_trans *tr, sql_schema *s, const char *tname)
{
	return (sql_type*)os_find_name(s->types, tr, tname);
}

sql_type *
sql_trans_bind_type(sql_trans *tr, sql_schema *c, const char *name)
{
	sql_type *t = NULL;

	if (tr->cat->schemas.set)
		for (node *n = tr->cat->schemas.set->h; n && !t; n = n->next) {
			sql_schema *s = n->data;

			t = find_sql_type(tr, s, name);
		}

	if (!t && c)
		t = find_sql_type(tr, c, name);
	return t;
}

sql_type *
sql_trans_find_type(sql_trans *tr, sql_schema *s, sqlid id)
{
	if (s) {
		sql_base *b = os_find_id(s->types, tr, id);
		if (b)
			return (sql_type*)b;
	} else {
		if (tr->cat->schemas.set) {
			for (node *n = tr->cat->schemas.set->h; n; n = n->next) {
				sql_schema *s = n->data;
				sql_base *b = os_find_id(s->types, tr, id);
				if (b)
					return (sql_type*)b;
			}
		}
	}
	return NULL;
}

sql_func *
find_sql_func(sql_trans *tr, sql_schema *s, const char *tname)
{
	return (sql_func*)os_find_name(s->funcs, tr, tname);
}

sql_func *
sql_trans_bind_func(sql_trans *tr, const char *name)
{
	node *n;
	sql_func *t = NULL;

	if (tr->cat->schemas.set)
		for (n = tr->cat->schemas.set->h; n && !t; n = n->next) {
			sql_schema *s = n->data;

			t = find_sql_func(tr, s, name);
		}
	if (!t)
		return NULL;
	return t;
}

sql_func *
sql_trans_find_func(sql_trans *tr, sqlid id)
{
	if (tr->cat->schemas.set) {
		for (node *n = tr->cat->schemas.set->h; n; n = n->next) {
			sql_schema *s = n->data;
			sql_base *b = os_find_id(s->funcs, tr, id);
			if (b)
				return (sql_func*)b;
		}
	}
	return NULL;
}

sql_trigger *
sql_trans_find_trigger(sql_trans *tr, sqlid id)
{
	if (tr->cat->schemas.set) {
		for (node *n = tr->cat->schemas.set->h; n; n = n->next) {
			sql_schema *s = n->data;
			sql_base *b = os_find_id(s->idxs, tr, id);
			if (b)
				return (sql_trigger*)b;
		}
	}
	return NULL;
}

void*
sql_values_list_element_validate_and_insert(void *v1, void *v2, void *tpe, int* res)
{
	sql_part_value* pt = (sql_part_value*) v1, *newp = (sql_part_value*) v2;
	sql_subtype *tp = (sql_subtype *) tpe;

	*res = ATOMcmp(tp->type->localtype, newp->value, pt->value);
	return *res == 0 ? pt : NULL;
}

void*
sql_range_part_validate_and_insert(void *v1, void *v2)
{
	sql_part* pt = (sql_part*) v1, *newp = (sql_part*) v2;
	int res1, res2, tpe = pt->tpe.type->localtype;
	const void *nil = ATOMnilptr(tpe);
	bool pt_down_all = false, pt_upper_all = false, newp_down_all = false, newp_upper_all = false, pt_min_max_same = false, newp_min_max_same = false;

	if (pt == newp) /* same pointer, skip (used in updates) */
		return NULL;

	assert(tpe == newp->tpe.type->localtype);
	if (is_bit_nil(pt->with_nills) || is_bit_nil(newp->with_nills)) /* if one partition holds all including nills, then conflicts */
		return pt;
	if (newp->with_nills && pt->with_nills) /* only one partition at most has null values */
		return pt;

	pt_down_all = !ATOMcmp(tpe, nil, pt->part.range.minvalue);
	pt_upper_all = !ATOMcmp(tpe, nil, pt->part.range.maxvalue);
	newp_down_all = !ATOMcmp(tpe, nil, newp->part.range.minvalue);
	newp_upper_all = !ATOMcmp(tpe, nil, newp->part.range.maxvalue);

	/* if one partition just holds NULL values, then there's no conflict */
	if ((newp_down_all && newp_upper_all && newp->with_nills) || (pt_down_all && pt_upper_all && pt->with_nills))
		return NULL;
	 /* holds all range, will always conflict */
	if ((pt_down_all && pt_upper_all && !pt->with_nills) || (newp_down_all && newp_upper_all && !newp->with_nills))
		return pt;

	pt_min_max_same = !ATOMcmp(tpe, pt->part.range.maxvalue, pt->part.range.minvalue);
	newp_min_max_same = !ATOMcmp(tpe, newp->part.range.maxvalue, newp->part.range.minvalue);

	if (pt_down_all) { /* from range min value until a value */
		res1 = ATOMcmp(tpe, pt->part.range.maxvalue, newp->part.range.minvalue);
		if (newp_down_all || (!newp_min_max_same && res1 > 0) || (newp_min_max_same && res1 >= 0))
			return pt;
		return NULL;
	}
	if (pt_upper_all) { /* from value until range max value */
		res1 = ATOMcmp(tpe, newp->part.range.maxvalue, pt->part.range.minvalue);
		if (newp_upper_all || (!newp_min_max_same && res1 > 0) || (newp_min_max_same && res1 >= 0))
			return pt;
		return NULL;
	}
	if (newp_down_all) { /* from range min value until a value */
		res1 = ATOMcmp(tpe, newp->part.range.maxvalue, pt->part.range.minvalue);
		if (pt_down_all || (!newp_min_max_same && res1 > 0) || (newp_min_max_same && res1 >= 0))
			return pt;
		return NULL;
	}
	if (newp_upper_all) { /* from value until range max value */
		res1 = ATOMcmp(tpe, pt->part.range.maxvalue, newp->part.range.minvalue);
		if (pt_upper_all || (!pt_min_max_same && res1 > 0) || (pt_min_max_same && res1 >= 0))
			return pt;
		return NULL;
	}

	/* Fallback into normal cases */
	res1 = ATOMcmp(tpe, newp->part.range.maxvalue, pt->part.range.minvalue);
	res2 = ATOMcmp(tpe, pt->part.range.maxvalue, newp->part.range.minvalue);
	/* overlap: y2 > x1 && x2 > y1 */
	if (((!newp_min_max_same && res1 > 0) || (newp_min_max_same && res1 >= 0)) && ((!pt_min_max_same && res2 > 0) || (pt_min_max_same && res2 >= 0)))
		return pt;
	return NULL;
}

void*
sql_values_part_validate_and_insert(void *v1, void *v2)
{
	sql_part* pt = (sql_part*) v1, *newp = (sql_part*) v2;
	list* b1 = pt->part.values, *b2 = newp->part.values;
	node *n1 = b1->h, *n2 = b2->h;
	int res;

	if (pt == newp) /* same pointer, skip (used in updates) */
		return NULL;

	assert(pt->tpe.type->localtype == newp->tpe.type->localtype);
	if (newp->with_nills && pt->with_nills)
		return pt; //check for nulls first

	while (n1 && n2) {
		sql_part_value *p1 = (sql_part_value *) n1->data, *p2 = (sql_part_value *) n2->data;
		res = ATOMcmp(pt->tpe.type->localtype, p1->value, p2->value);
		if (!res) { //overlap -> same value in both partitions
			return pt;
		} else if (res < 0) {
			n1 = n1->next;
		} else {
			n2 = n2->next;
		}
	}
	return NULL;
}

sql_part *
partition_find_part(sql_trans *tr, sql_table *pt, sql_part *pp)
{
	sql_schema *s = pt->s;

	struct os_iter *oi = os_iterator(s->tables, tr, NULL);
	for (sql_base *b = oi_next(oi); b; b = oi_next(oi)) {
		sql_table *t = (sql_table*)b;

		if (!isMergeTable(t) || !t->members.set)
			continue;
		for(node *m = t->members.set->h; m; m=m->next) {
			sql_part *p = m->data;

			if (pp) {
				if (p == pp)
					pp = NULL;
				continue;
			}
			if (p->base.id == pt->base.id)
				return p;
		}
	}
	return NULL;
}

int
nested_mergetable(sql_trans *tr, sql_table *mt, const char *sname, const char *tname)
{
	if (strcmp(mt->s->base.name, sname) == 0 && strcmp(mt->base.name, tname) == 0)
		return 1;
	if (isPartition(mt)) {
		for( sql_part *parent = partition_find_part(tr, mt, NULL); parent; parent = partition_find_part(tr, mt, parent)) {
			if (nested_mergetable(tr, parent->t, sname, tname))
				return 1;
		}
	}
	return 0;
}
