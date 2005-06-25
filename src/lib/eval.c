/*
 * $Id$
 *
 * Copyright (c) 2003, Raphael Manfredi
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

/**
 * @ingroup lib
 * @file
 *
 * String evaluation.
 *
 * @author Raphael Manfredi
 * @date 2003
 */

#include "common.h"

RCSID("$Id$");

#include <pwd.h>

#include "eval.h"
#include "atoms.h"
#include "misc.h"			/* For g_strlcpy() */
#include "override.h"		/* Must be the last header included */

#define MAX_STRING	1024	/**< Max length for substitution */

static guint32 common_dbg = 0;	/* XXX -- need to init lib's props --RAM */

static GHashTable *constants;
static gchar *home;

static gchar *get_home(void);
static gchar *get_variable(gchar *s, gchar **end);

/**
 * Create a constant string, or reuse an existing one if possible.
 *
 * @returns a string atom.
 */
static gchar *
constant_make(gchar *s)
{
	gchar *v;

	v = (gchar *) g_hash_table_lookup(constants, s);
	if (v != NULL)
		return v;			/* Already exists */

	v = atom_str_get(s);
	g_hash_table_insert(constants, v, v);

	return v;
}

/**
 * Initialize string evaluation.
 */
void
eval_init(void)
{
	constants = g_hash_table_new(g_str_hash, g_str_equal);
	home = get_home();
}

static void
constants_free_kv(gpointer key,
	gpointer unused_val, gpointer unused_x)
{
	(void) unused_val;
	(void) unused_x;
	atom_str_free(key);
}

/**
 * Cleanup local structures at shutdown time.
 */
void
eval_close(void)
{
	atom_str_free(home);

	g_hash_table_foreach(constants, constants_free_kv, NULL);
	g_hash_table_destroy(constants);
}

/**
 * Insert value `val' at beginning of string `start'.
 *
 * The string `start' is held in a buffer capable of holding a string of
 * `maxlen' bytes, and the string is currently `len' bytes long, with `start'
 * being at the offset `off' within buffer.
 *
 * @return the pointer right after the inserted value.
 */
static gchar *
insert_value(gchar *val, gchar *start, gint off,
	gint len, gint maxlen)
{
	gint vlen = strlen(val);

	g_assert(len <= maxlen);
	g_assert(off >= 0 && off <= len);

	if (len + vlen > maxlen) {
		g_warning("ignoring variable substitution text \"%s\"", val);
		return start;
	}

	memmove(start + vlen, start, len + 1 - off);
	memmove(start, val, vlen);

	return start + vlen;
}

/**
 * Substitutes variables from string:
 *
 * . The leading "~" is replaced by the home directory.
 * . Variables like "$PATH" or "${PATH}" are replaced by their value, as
 *   fetched from the environment, or the empty string if not found.
 *
 * If given a NULL input, we return NULL.
 *
 * @return string atom, which is not meant to be freed until exit time.
 */
gchar *
eval_subst(const gchar *str)
{
	gchar buf[MAX_STRING];
	gchar *p;
	gchar *end = buf + sizeof(buf);
	size_t len;
	gchar c;

	if (str == NULL)
		return NULL;

	len = g_strlcpy(buf, str, sizeof(buf));
	if (len >= sizeof(buf)) {
		g_warning("eval_subst: string too large for substitution (%d bytes)",
			(int) len);
		return constant_make((gchar *) str);
	}


	if (common_dbg > 3)
		printf("eval_subst: on entry: \"%s\"\n", buf);

	for (p =  buf, c = *p++; c; c = *p++) {
		gchar *val = NULL;
		gchar *start = p - 1;

		if (c == '~' && start == buf) {		/* Leading ~ only */
			val = home;
			memmove(start, start + 1, len - (start - buf));
			len--;

			g_assert((ssize_t) len >= 0);

		} else if (c == '$') {
			gchar *after;

			val = get_variable(p, &after);
			memmove(start, after, len + 1 - (after - buf));
			len -= after - start;		/* Also removing leading '$' */

			g_assert((ssize_t) len >= 0);
		}


		if (val != NULL) {
			gchar *next = insert_value(
				val, start, start - buf, len, sizeof(buf) - 1);

			len += next - start;
			p = next;

			g_assert(len < sizeof(buf));
			g_assert(p < end);
		}

		g_assert(p <= (buf + len));
	}

	if (common_dbg > 3)
		printf("eval_subst: on exit: \"%s\"\n", buf);

	g_assert(len == strlen(buf));

	return constant_make(buf);
}

/**
 * Compute the user's home directory.
 * Uses the HOME environment variable first, then the entry from /etc/passwd.
 *
 * @return string atom.
 */
static gchar *
get_home(void)
{
	gchar *v;
	struct passwd *pp;

	v = getenv("HOME");
	if (v != NULL)
		return atom_str_get(v);

	pp = getpwuid(getuid());
	if (pp != NULL)
		return atom_str_get(pp->pw_dir);

	g_warning("Could not determine home directory");
	return atom_str_get("/");
}

/**
 * Extract variable name from string `s', then fetch value from environment.
 *
 * @return variable's value, or "" if not found and set `end' to the address
 * of the character right after the variable name.
 */
static gchar *get_variable(gchar *s, gchar **end)
{
	guchar *p = (guchar *) s;
	guchar c;
	gchar *name = s;
	gchar *value;
	gboolean end_brace = FALSE;

	/*
	 * Grab variable's name.
	 */

	if (*p == '{') {
		p++;
		name++;
		end_brace = TRUE;
	}

	while ((c = *p)) {
		if (!isalnum(c) && c != '_')
			break;
		p++;
	}

	if (end_brace && *p == '}')
		*end = (gchar *) (p + 1);
	else
		*end = (gchar *) p;

	/*
	 * Get value from environment.
	 */

	c = *p;
	*p = '\0';
	value = getenv(name);

	if (value == NULL)
		value = "";

	if (common_dbg > 4)
		printf("variable \"%s\" is \"%s\"\n", name, value);

	*p = c;

	return value;
}

/* vi: set ts=4 sw=4 cindent: */
