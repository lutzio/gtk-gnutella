/*
 * $Id$
 *
 * Copyright (c) 2002-2003, Richard Eckart
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

#ifndef _gtk_gtk_missing_h_
#define _gtk_gtk_missing_h_

#include "gui.h"
#include <gtk/gtk.h>

#include "lib/prop.h"

/*
 * GtkProgressBar
 *
 * Make Gtk1 and Gtk2 versions useable using the same interface.
 */
#ifndef USE_GTK2
#define gtk_progress_bar_set_fraction(pb, val) \
    gtk_progress_set_percentage(GTK_PROGRESS(pb), val)
void gtk_progress_bar_set_text(GtkProgressBar *pb, const gchar *text);
gint gtk_paned_get_position(GtkPaned *paned);
#endif

/*
 * GtkSpinButton
 */
#ifndef USE_GTK2
#define gtk_spin_button_get_value(w) \
    _gtk_spin_button_get_value(w)
#endif
gdouble _gtk_spin_button_get_value(GtkSpinButton *);

/*
 * GtkCList
 */
#ifdef USE_GTK1
void gtk_clist_set_column_name(GtkCList * clist, gint col, gchar * t);
GSList *clist_collect_data(GtkCList *clist, gboolean allow_null,
    GCompareFunc cfn);
void gtk_clist_save_visibility(GtkCList *clist, property_t prop);
void gtk_clist_restore_visibility(GtkCList *clist, property_t prop);
#endif /* USE_GTK1 */

/*
 * GtkCTree
 */
#ifdef USE_GTK1
void gtk_ctree_fast_move (GtkCTree *ctree, GtkCTreeNode *node,
	GtkCTreeNode *new_sibling);
gint gtk_ctree_count_node_children(GtkCTree *ctree, GtkCTreeNode *parent);

#define GTK_CTREE_NODE_HAS_CHILDREN(n) \
    ((n) ? (GTK_CTREE_ROW(n)->children) != NULL : FALSE)
#define GTK_CTREE_NODE_SIBLING(n) \
    ((n) ? (GTK_CTREE_ROW(n)->sibling) : NULL)
#define GTK_CTREE_NODE_PARENT(n) \
    ((n) ? (GTK_CTREE_ROW(n)->parent) : NULL)
#endif /* USE_GTK1 */

/*
 * GtkLabel
 */
void gtk_label_printf(GtkLabel *, const gchar *fmt, ...) G_GNUC_PRINTF(2, 3);

/*
 * GtkEntry
 */
void gtk_entry_printf(GtkEntry *, const gchar *fmt, ...) G_GNUC_PRINTF(2, 3);

/*
 * GtkEditable
 */
guint32 gtk_editable_get_value_as_uint(GtkEditable *editable);

/*
 * GtkCombo
 */
void gtk_combo_init_choices(
    GtkCombo* combo, GtkSignalFunc func, prop_def_t *def, gpointer user_data);

/*
 * GtkOptionMenu
 */
void option_menu_select_item_by_data(GtkWidget *m, gpointer *d);
gpointer option_menu_get_selected_data(GtkWidget *m);

/*
 * GtkWidget
 */
void gtk_mass_widget_set_sensitive(GtkWidget *tl, gchar *list[], gboolean b);

/* 
 * GtkTreeView
 */
#ifdef USE_GTK2
GtkTreeIter *w_tree_iter_new(void);
GtkTreeIter *w_tree_iter_copy(GtkTreeIter *iter);
void w_tree_iter_free(GtkTreeIter *iter);
GSList *tree_selection_collect_data(GtkTreeSelection *tsel, GCompareFunc cfn);
void tree_view_save_widths(GtkTreeView *treeview, property_t prop);
void tree_view_save_visibility(GtkTreeView *treeview, property_t prop);
void tree_view_restore_visibility(GtkTreeView *treeview, property_t prop);
void tree_view_restore_widths(GtkTreeView *treeview, property_t prop);
void set_tooltips_keyboard_mode(GtkWidget *widget, gboolean on);
void widget_force_tooltip(GtkWidget *widget);
#endif /* USE_GTK2 */

/*
 * GtkListStore
 */
#define GTK_LIST_STORE_SET(store, iter, ...) \
	gtk_list_store_set(store, iter, ## __VA_ARGS__, (-1))

/*
 * GtkTreeStore
 */
#define GTK_TREE_STORE_SET(store, iter, ...) \
	gtk_tree_store_set(store, iter, ## __VA_ARGS__, (-1))

/*
 * GtkTreeModel
 */
#define GTK_TREE_MODEL_GET(model, iter, ...) \
	gtk_tree_model_get(model, iter, ## __VA_ARGS__, (-1))

gint gtk_main_flush(void);
GtkWidget *menu_new_item_with_data(GtkMenu *m, gchar *l, gpointer d );
GtkWidget *radiobutton_get_active_in_group(GtkRadioButton *rb);

void gtk_widget_fix_width(GtkWidget *w, GtkWidget *l, guint chars, guint extra);

#endif	/* _gtk_gtk_missing_h_ */
/* vi: set ts=4 sw=4 cindent: */
