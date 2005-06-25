/*
 * Copyright (c) 2004, Jeroen Asselman
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
 * @ingroup undoc
 * @file
 *
 * Glue between Gtk-Gnutella and the G2 'lib'
 *
 * @author Jeroen Asselman
 * @date 2004
 */

#ifndef _g2node_h_
#define _g2node_h_

#include "gnutella.h"
#include "pmsg.h"

gboolean g2_node_read(struct gnutella_node *n, pmsg_t *mb);

#endif /* _g2node_h_ */

/* vi: set ts=4 sw=4 cindent: */
