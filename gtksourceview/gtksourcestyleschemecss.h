/*
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2023 - Sébastien Wilmet <swilmet@gnome.org>
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

#ifndef GTK_SOURCE_STYLE_SCHEME_CSS_H
#define GTK_SOURCE_STYLE_SCHEME_CSS_H

#include <gtk/gtk.h>
#include "gtksourcestylescheme.h"

G_BEGIN_DECLS

#define GTK_SOURCE_TYPE_STYLE_SCHEME_CSS             (_gtk_source_style_scheme_css_get_type ())
#define GTK_SOURCE_STYLE_SCHEME_CSS(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_SOURCE_TYPE_STYLE_SCHEME_CSS, GtkSourceStyleSchemeCss))
#define GTK_SOURCE_STYLE_SCHEME_CSS_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_SOURCE_TYPE_STYLE_SCHEME_CSS, GtkSourceStyleSchemeCssClass))
#define GTK_SOURCE_IS_STYLE_SCHEME_CSS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_SOURCE_TYPE_STYLE_SCHEME_CSS))
#define GTK_SOURCE_IS_STYLE_SCHEME_CSS_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_SOURCE_TYPE_STYLE_SCHEME_CSS))
#define GTK_SOURCE_STYLE_SCHEME_CSS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_SOURCE_TYPE_STYLE_SCHEME_CSS, GtkSourceStyleSchemeCssClass))

typedef struct _GtkSourceStyleSchemeCss         GtkSourceStyleSchemeCss;
typedef struct _GtkSourceStyleSchemeCssClass    GtkSourceStyleSchemeCssClass;
typedef struct _GtkSourceStyleSchemeCssPrivate  GtkSourceStyleSchemeCssPrivate;

struct _GtkSourceStyleSchemeCss
{
	GObject parent;

	GtkSourceStyleSchemeCssPrivate *priv;
};

struct _GtkSourceStyleSchemeCssClass
{
	GObjectClass parent_class;
};

G_GNUC_INTERNAL
GType		_gtk_source_style_scheme_css_get_type	(void);

G_GNUC_INTERNAL
GtkSourceStyleSchemeCss *
		_gtk_source_style_scheme_css_new	(GtkSourceStyleScheme *scheme);

G_GNUC_INTERNAL
void		_gtk_source_style_scheme_css_apply	(GtkSourceStyleSchemeCss *scheme_css,
							 GtkWidget               *widget);

G_GNUC_INTERNAL
void		_gtk_source_style_scheme_css_unapply	(GtkSourceStyleSchemeCss *scheme_css,
							 GtkWidget               *widget);

G_END_DECLS

#endif /* GTK_SOURCE_STYLE_SCHEME_CSS_H */
