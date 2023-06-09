/*
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2003-2007 - Paolo Maggi <paolo.maggi@polito.it>
 *
 * GtkSourceView is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * GtkSourceView is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GTK_SOURCE_STYLE_SCHEME_MANAGER_H
#define GTK_SOURCE_STYLE_SCHEME_MANAGER_H

#if !defined (GTK_SOURCE_H_INSIDE) && !defined (GTK_SOURCE_COMPILATION)
#error "Only <gtksourceview/gtksource.h> can be included directly."
#endif

#include <gtksourceview/gtksourcestylescheme.h>

G_BEGIN_DECLS

#define GTK_SOURCE_TYPE_STYLE_SCHEME_MANAGER		(gtk_source_style_scheme_manager_get_type ())
#define GTK_SOURCE_STYLE_SCHEME_MANAGER(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_SOURCE_TYPE_STYLE_SCHEME_MANAGER, GtkSourceStyleSchemeManager))
#define GTK_SOURCE_STYLE_SCHEME_MANAGER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), GTK_SOURCE_TYPE_STYLE_SCHEME_MANAGER, GtkSourceStyleSchemeManagerClass))
#define GTK_SOURCE_IS_STYLE_SCHEME_MANAGER(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_SOURCE_TYPE_STYLE_SCHEME_MANAGER))
#define GTK_SOURCE_IS_STYLE_SCHEME_MANAGER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_SOURCE_TYPE_STYLE_SCHEME_MANAGER))
#define GTK_SOURCE_STYLE_SCHEME_MANAGER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), GTK_SOURCE_TYPE_STYLE_SCHEME_MANAGER, GtkSourceStyleSchemeManagerClass))

typedef struct _GtkSourceStyleSchemeManager		GtkSourceStyleSchemeManager;
typedef struct _GtkSourceStyleSchemeManagerClass	GtkSourceStyleSchemeManagerClass;
typedef struct _GtkSourceStyleSchemeManagerPrivate	GtkSourceStyleSchemeManagerPrivate;

struct _GtkSourceStyleSchemeManager
{
	GObject parent;

	GtkSourceStyleSchemeManagerPrivate *priv;
};

struct _GtkSourceStyleSchemeManagerClass
{
	GObjectClass parent_class;

	/* Padding for future expansion */
	gpointer padding[10];
};

G_MODULE_EXPORT
GType			gtk_source_style_scheme_manager_get_type		(void);

G_MODULE_EXPORT
GtkSourceStyleSchemeManager *
			gtk_source_style_scheme_manager_new			(void);

G_MODULE_EXPORT
GtkSourceStyleSchemeManager *
			gtk_source_style_scheme_manager_get_default		(void);

G_MODULE_EXPORT
void			gtk_source_style_scheme_manager_set_search_path		(GtkSourceStyleSchemeManager  *manager,
										 gchar                       **path);

G_MODULE_EXPORT
void			gtk_source_style_scheme_manager_append_search_path	(GtkSourceStyleSchemeManager *manager,
										 const gchar                 *path);

G_MODULE_EXPORT
void			gtk_source_style_scheme_manager_prepend_search_path	(GtkSourceStyleSchemeManager *manager,
										 const gchar                 *path);

G_MODULE_EXPORT
const gchar * const *	gtk_source_style_scheme_manager_get_search_path		(GtkSourceStyleSchemeManager *manager);

G_MODULE_EXPORT
void			gtk_source_style_scheme_manager_force_rescan		(GtkSourceStyleSchemeManager *manager);

G_MODULE_EXPORT
GList *			gtk_source_style_scheme_manager_get_schemes		(GtkSourceStyleSchemeManager *manager);

G_MODULE_EXPORT
GtkSourceStyleScheme *	gtk_source_style_scheme_manager_get_scheme		(GtkSourceStyleSchemeManager *manager,
										 const gchar                 *scheme_id);

G_GNUC_INTERNAL
GtkSourceStyleSchemeManager *
			_gtk_source_style_scheme_manager_peek_default		(void);

G_END_DECLS

#endif /* GTK_SOURCE_STYLE_SCHEME_MANAGER_H */
