/*
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2003 - Paolo Maggi <paolo.maggi@polito.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GTK_SOURCE_STYLE_SCHEME_H
#define GTK_SOURCE_STYLE_SCHEME_H

#if !defined (GTK_SOURCE_H_INSIDE) && !defined (GTK_SOURCE_COMPILATION)
#error "Only <gtksourceview/gtksource.h> can be included directly."
#endif

#include <gtksourceview/gtksourcestyle.h>

G_BEGIN_DECLS

#define GTK_SOURCE_TYPE_STYLE_SCHEME             (gtk_source_style_scheme_get_type ())
#define GTK_SOURCE_STYLE_SCHEME(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_SOURCE_TYPE_STYLE_SCHEME, GtkSourceStyleScheme))
#define GTK_SOURCE_STYLE_SCHEME_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_SOURCE_TYPE_STYLE_SCHEME, GtkSourceStyleSchemeClass))
#define GTK_SOURCE_IS_STYLE_SCHEME(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_SOURCE_TYPE_STYLE_SCHEME))
#define GTK_SOURCE_IS_STYLE_SCHEME_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_SOURCE_TYPE_STYLE_SCHEME))
#define GTK_SOURCE_STYLE_SCHEME_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_SOURCE_TYPE_STYLE_SCHEME, GtkSourceStyleSchemeClass))

typedef struct _GtkSourceStyleScheme         GtkSourceStyleScheme;
typedef struct _GtkSourceStyleSchemeClass    GtkSourceStyleSchemeClass;
typedef struct _GtkSourceStyleSchemePrivate  GtkSourceStyleSchemePrivate;

struct _GtkSourceStyleScheme
{
	GObject parent;

	GtkSourceStyleSchemePrivate *priv;
};

struct _GtkSourceStyleSchemeClass
{
	GObjectClass parent_class;

	/* Padding for future expansion */
	gpointer padding[10];
};

/**
 * GtkSourceStyleSchemeKind:
 * @GTK_SOURCE_STYLE_SCHEME_KIND_LIGHT: Light style.
 * @GTK_SOURCE_STYLE_SCHEME_KIND_DARK: Dark style.
 * @GTK_SOURCE_STYLE_SCHEME_KIND_LIGHT_ONLY: Light style. Supports only a light
 *   GTK theme.
 * @GTK_SOURCE_STYLE_SCHEME_KIND_DARK_ONLY: Dark style. Supports only a dark GTK
 *   theme.
 *
 * The kind (or category) of a #GtkSourceStyleScheme.
 *
 * Since: 299.2
 */
typedef enum _GtkSourceStyleSchemeKind
{
	GTK_SOURCE_STYLE_SCHEME_KIND_LIGHT,
	GTK_SOURCE_STYLE_SCHEME_KIND_DARK,
	GTK_SOURCE_STYLE_SCHEME_KIND_LIGHT_ONLY,
	GTK_SOURCE_STYLE_SCHEME_KIND_DARK_ONLY,
} GtkSourceStyleSchemeKind;

G_MODULE_EXPORT
GType			gtk_source_style_scheme_get_type		(void);

G_MODULE_EXPORT
const gchar *		gtk_source_style_scheme_get_id			(GtkSourceStyleScheme *scheme);

G_MODULE_EXPORT
const gchar *		gtk_source_style_scheme_get_name		(GtkSourceStyleScheme *scheme);

G_MODULE_EXPORT
const gchar *		gtk_source_style_scheme_get_description		(GtkSourceStyleScheme *scheme);

G_MODULE_EXPORT
GtkSourceStyleSchemeKind gtk_source_style_scheme_get_kind		(GtkSourceStyleScheme *scheme);

G_MODULE_EXPORT
const gchar *		gtk_source_style_scheme_get_filename		(GtkSourceStyleScheme *scheme);

G_MODULE_EXPORT
GtkSourceStyle *	gtk_source_style_scheme_get_style		(GtkSourceStyleScheme *scheme,
									 const gchar          *style_id);

G_GNUC_INTERNAL
GtkSourceStyleScheme *	_gtk_source_style_scheme_new_from_file		(const gchar *filename);

G_END_DECLS

#endif /* GTK_SOURCE_STYLE_SCHEME_H */
