/*
 * $Id$
 *
 * Copyright (c) 2002, Raphael Manfredi
 *
 * Atom management.
 *
 * An atom is a single piece of information that is likely to be shared
 * and which is therefore only allocated once: all other instances point
 * to the common object.
 *
 *----------------------------------------------------------------------
 * This file is part of gtk-gnutella.
 *
 *  gtk-gnutella is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  gtk-gnutella is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gtk-gnutella; if not, write to the Free Software
 *  Foundation, Inc.:
 *      59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *----------------------------------------------------------------------
 */

#include <string.h>

#include "common.h"

/*
 * Atoms are ref-counted.
 *
 * The reference count is held at the beginning of the data arena.
 * What we return to the outside is a pointer to arena[], not to
 * the atom structure.
 */
struct atom {
	gint refcnt;				/* Amount of references */
	guchar arena[1];			/* Start of user arena */
};

#define ARENA_OFFSET	G_STRUCT_OFFSET(struct atom, arena)

typedef gint (*len_func_t)(gconstpointer v);
typedef gchar *(*str_func_t)(gconstpointer v);

/*
 * Description of atom types.
 */
struct table_desc {
	gchar *type;				/* Type of atoms */
	GHashTable *table;			/* Table of atoms: "atom value" => 1 */
	GHashFunc hash_func;		/* Hashing function for atoms */
	GCompareFunc eq_func;		/* Atom equality function */
	len_func_t len_func;		/* Atom length function */
	str_func_t str_func;		/* Atom to human-readable string */
};

#define public

static gint str_len(gconstpointer v);
static gchar *str_str(gconstpointer v);
public guint guid_hash(gconstpointer key);
public gint guid_eq(gconstpointer a, gconstpointer b);
static gint guid_len(gconstpointer v);
static gchar *guid_str(gconstpointer v);
public guint sha1_hash(gconstpointer key);
public gint sha1_eq(gconstpointer a, gconstpointer b);
static gint sha1_len(gconstpointer v);
static gchar *sha1_str(gconstpointer v);
static gint int_len(gconstpointer v);
static gchar *int_str(gconstpointer v);

#undef public

/*
 * The set of all atom types we know about.
 */
struct table_desc atoms[] = {
	{ "String",	NULL,	g_str_hash,	g_str_equal, str_len,	str_str	},	/* 0 */
	{ "GUID",	NULL,	guid_hash,	guid_eq,	 guid_len,	guid_str},	/* 1 */
	{ "SHA1",	NULL,	sha1_hash,	sha1_eq,	 sha1_len,	sha1_str},	/* 2 */
	{ "int",	NULL,	g_int_hash,	g_int_equal, int_len,	int_str},	/* 3 */
};

#define COUNT(x)	(sizeof(x) / sizeof(x[0]))

/*
 * str_len
 *
 * Returns length of string + trailing NUL.
 */
static gint str_len(gconstpointer v)
{
	gchar *p = (gchar *) v;

	while (*p++)
		/* empty */;

	return p - (gchar *) v;
}

/*
 * str_str
 *
 * Returns printable form of a string, i.e. self.
 */
static gchar *str_str(gconstpointer v)
{
	return (gchar *) v;
}

/*
 * binary_hash
 *
 * Hash `len' bytes (multiple of 4) starting from `key'.
 */
static guint binary_hash(gconstpointer key, gint len)
{
	guchar *buf = (guchar *) key;
	gint i;
	guint hash = 0;

	g_assert(0 == (len & 0x3));		/* Multiple of 4 */

	for (i = 0; i < len; i += 4) {
		guint v =
			(buf[i])            |
			(buf[i + 1] << 8)   |
			(buf[i + 2] << 16)  |
			(buf[i + 3] << 24);

		hash ^= v;
	}

	return hash;
}

/*
 * guid_hash
 *
 * Hash a GUID (16 bytes).
 */
guint guid_hash(gconstpointer key)
{
	return binary_hash(key, 16);
}

/*
 * guid_eq
 *
 * Test two GUIDs for equality.
 */
gint guid_eq(gconstpointer a, gconstpointer b)
{
	return 0 == memcmp(a, b, 16);
}

/*
 * guid_len
 *
 * Returns length of GUID.
 */
static gint guid_len(gconstpointer v)
{
	return 16;
}

/*
 * guid_str
 *
 * Returns printable form of a GUID, as pointer to static data.
 */
static gchar *guid_str(gconstpointer v)
{
	return guid_hex_str((guchar *) v);
}

/*
 * sha1_hash
 *
 * Hash a SHA1 (20 bytes).
 *
 * NB: This routine is visible for the download mesh.
 */
guint sha1_hash(gconstpointer key)
{
	return binary_hash(key, 20);
}

/*
 * sha1_eq
 *
 * Test two SHA1s for equality.
 *
 * NB: This routine is visible for the download mesh.
 */
gint sha1_eq(gconstpointer a, gconstpointer b)
{
	return 0 == memcmp(a, b, 20);
}

/*
 * sha1_len
 *
 * Returns length of SHA1.
 */
static gint sha1_len(gconstpointer v)
{
	return 20;
}

/*
 * sha1_str
 *
 * Returns printable form of a SHA1, as pointer to static data.
 */
static gchar *sha1_str(gconstpointer v)
{
	return sha1_base32((guchar *) v);
}

/*
 * int_len
 *
 * Returns length of an int.
 */
static gint int_len(gconstpointer v)
{
	return sizeof(gint);
}

/*
 * int_str
 *
 * Returns printable form of an int, as pointer to static data.
 */
static gchar *int_str(gconstpointer v)
{
	static gchar fmt[32];

	g_snprintf(fmt, sizeof(fmt), "%d/%u", *(gint *) v, *(guint *) v);
	fmt[sizeof(fmt)-1] = '\0';

	return fmt;
}

/*
 * atoms_init
 *
 * Initialize atom structures.
 */
void atoms_init(void)
{
	gint i;

	for (i = 0; i < COUNT(atoms); i++) {
		struct table_desc *td = &atoms[i];

		td->table = g_hash_table_new(td->hash_func, td->eq_func);
	}
}

/*
 * Under USE_DMALLOC, we don't use atom_get() and atom_free().
 */

#ifndef USE_DMALLOC
/*
 * atom_get
 *
 * Get atom of given `type', whose value is `key'.
 * If the atom does not exist yet, `key' is cloned and makes up the new atom.
 *
 * Returns the atom's value.
 */
gpointer atom_get(gint type, gconstpointer key)
{
	struct table_desc *td;
	gboolean found;
	gpointer value;
	gpointer x;
	struct atom *a;
	gint len;

	g_assert(type >= 0 && type < COUNT(atoms));

	td = &atoms[type];		/* Where atoms of this type are held */

	/* 
	 * If atom exists, increment ref count and return it.
	 */

	found = g_hash_table_lookup_extended(td->table, key, &value, &x);

	if (found) {
		a = (struct atom *) (value - ARENA_OFFSET);

		g_assert(a->refcnt > 0);

		a->refcnt++;
		return value;
	}

	/*
	 * Create new atom.
	 */

	len = (*td->len_func)(key);

	a = g_malloc(ARENA_OFFSET + len);
	a->refcnt = 1;
	memcpy(a->arena, key, len);

	/*
	 * Insert atom in table.
	 */

	g_hash_table_insert(td->table, a->arena, (gpointer) 1);

	return a->arena;
}

/*
 * atom_free
 *
 * Remove one reference from atom. 
 * Dispose of atom if nobody references it anymore.
 */
void atom_free(gint type, gconstpointer key)
{
	struct table_desc *td;
	gboolean found;
	gpointer value;
	gpointer x;
	struct atom *a;

	g_assert(type >= 0 && type < COUNT(atoms));

	td = &atoms[type];		/* Where atoms of this type are held */

	found = g_hash_table_lookup_extended(td->table, key, &value, &x);

	g_assert(found);
	g_assert(value == key);

	a = (struct atom *) (key - ARENA_OFFSET);

	g_assert(a->refcnt > 0);

	/*
	 * Dispose of atom when its reference count reaches 0.
	 */

	if (--a->refcnt == 0) {
		g_hash_table_remove(td->table, key);
		g_free(a);
	}
}
#endif	/* !USE_DMALLOC */

/*
 * atom_warn_free
 *
 * Free atom entry, warning about it.
 */
static gboolean atom_warn_free(gpointer key, gpointer value, gpointer udata)
{
	struct atom *a = (struct atom *) (key - ARENA_OFFSET);
	struct table_desc *td = (struct table_desc *) udata;

	g_warning("freeing remaining %s atom 0x%lx, refcnt=%d: \"%s\"",
		td->type, (glong) key, a->refcnt, (*td->str_func)(key));

	g_free(a);

	return TRUE;
}

/*
 * atoms_close
 *
 * Shutdown atom structures, freeing all remaining atoms.
 */
void atoms_close(void)
{
	gint i;

	for (i = 0; i < COUNT(atoms); i++) {
		struct table_desc *td = &atoms[i];

		g_hash_table_foreach_remove(td->table, atom_warn_free, td);
		g_hash_table_destroy(td->table);
	}
}

/*
 * Under USE_DMALLOC, we need the following.
 */

#ifdef USE_DMALLOC

gint atom_dmalloc_len;
gpointer atom_dmalloc_ptr;

/*
 * atom_length
 *
 * Return the length of the atom key.
 */
gint atom_length(gint type, gconstpointer key)
{
	struct table_desc *td;

	g_assert(type >= 0 && type < COUNT(atoms));

	td = &atoms[type];		/* Where atoms of this type are held */

	return (*td->len_func)(key);
}
#endif	/* USE_DMALLOC */

