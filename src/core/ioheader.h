/*
 * $Id$
 *
 * Copyright (c) 2002-2003, Raphael Manfredi
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
 * @ingroup core
 * @file
 *
 * Asynchronous I/O header parsing routines.
 *
 * @author Raphael Manfredi
 * @date 2002-2003
 */

#ifndef _core_ioheader_h_
#define _core_ioheader_h_

#include <glib.h>

struct io_header;
struct header;
struct getline;
struct bsched;
struct gnutella_socket;

/**
 * This structure holds all the per-resource callbacks that can be used
 * during header processing in case something goes wrong.
 */
struct io_error {
	void (*line_too_long)(gpointer resource);
	void (*header_error_tell)(gpointer resource, gint error);	/**< Optional */
	void (*header_error)(gpointer resource, gint error);
	void (*input_exception)(gpointer resource);
	void (*input_buffer_full)(gpointer resource);
	void (*header_read_error)(gpointer resource, gint error);
	void (*header_read_eof)(gpointer resource);
	void (*header_extra_data)(gpointer resource);
};

typedef void (*io_done_cb_t)(gpointer resource, struct header *header);
typedef void (*io_start_cb_t)(gpointer resource);

/*
 * Parsing flags.
 */

#define IO_HEAD_ONLY		0x00000001	/**< No data expected after EOH */
#define IO_SAVE_FIRST		0x00000002	/**< Save 1st line in socket's getline */
#define IO_SINGLE_LINE		0x00000004	/**< Get one line only, then process */
#define IO_3_WAY			0x00000008	/**< In 3-way handshaking */
#define IO_SAVE_HEADER		0x00000010	/**< Save header text for later perusal */

/*
 * Public interface
 */

void io_free(gpointer opaque);
struct header *io_header(gpointer opaque);
struct getline *io_getline(gpointer opaque);
gchar *io_gettext(gpointer opaque);

void io_get_header(
	gpointer resource,				/**< Resource for which we're reading headers */
	gpointer *io_opaque,			/**< Field address in resource's structure */
	struct bsched *bs,				/**< B/w scheduler from which we read */
	struct gnutella_socket *s,		/**< Socket from which we're reading */
	gint flags,						/**< I/O parsing flags */
	io_done_cb_t done,				/**< Mandatory: final callback when all done */
	io_start_cb_t start,			/**< Optional: called when reading 1st byte */
	const struct io_error *error);	/**< Mandatory: error callbacks for resource */

void io_continue_header(
	gpointer opaque,				/**< Existing header parsing context */
	gint flags,						/**< New I/O parsing flags */
	io_done_cb_t done,				/**< Mandatory: final callback when all done */
	io_start_cb_t start);			/**< Optional: called when reading 1st byte */

#endif	/* _core_ioheader_h_ */

/* vi: set ts=4 sw=4 cindent: */

