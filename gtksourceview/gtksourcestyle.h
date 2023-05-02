/*
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2003 - Paolo Maggi <paolo.maggi@polito.it>
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

#ifndef GTK_SOURCE_STYLE_H
#define GTK_SOURCE_STYLE_H

#if !defined (GTK_SOURCE_H_INSIDE) && !defined (GTK_SOURCE_COMPILATION)
#error "Only <gtksourceview/gtksource.h> can be included directly."
#endif

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTK_SOURCE_TYPE_STYLE             (gtk_source_style_get_type ())
#define GTK_SOURCE_STYLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_SOURCE_TYPE_STYLE, GtkSourceStyle))
#define GTK_SOURCE_IS_STYLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_SOURCE_TYPE_STYLE))
#define GTK_SOURCE_STYLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_SOURCE_TYPE_STYLE, GtkSourceStyleClass))
#define GTK_SOURCE_IS_STYLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_SOURCE_TYPE_STYLE))
#define GTK_SOURCE_STYLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_SOURCE_TYPE_STYLE, GtkSourceStyleClass))

typedef struct _GtkSourceStyle		GtkSourceStyle;
typedef struct _GtkSourceStyleClass	GtkSourceStyleClass;
typedef struct _GtkSourceStyleData	GtkSourceStyleData;

/**
 * GtkSourceStyleData:
 * @foreground_color: Equivalent to #GtkTextTag:foreground-rgba.
 * @background_color: Equivalent to #GtkTextTag:background-rgba.
 * @underline_color: Equivalent to #GtkTextTag:underline-rgba.
 * @paragraph_background_color: Equivalent to #GtkTextTag:paragraph-background-rgba.
 * @scale: Equivalent to #GtkTextTag:scale.
 * @underline: Equivalent to #GtkTextTag:underline.
 * @italic: For #GtkTextTag:style.
 * @bold: For #GtkTextTag:weight.
 * @strikethrough: Equivalent to #GtkTextTag:strikethrough.
 * @use_foreground_color: Whether @foreground_color can be taken into account.
 * @use_background_color: Whether @background_color can be taken into account.
 * @use_underline_color: Whether @underline_color can be taken into account.
 * @use_paragraph_background_color: Whether @paragraph_background_color can be taken into account.
 * @use_scale: Whether @scale can be taken into account.
 * @use_underline: Whether @underline can be taken into account.
 * @use_italic: Whether @italic can be taken into account.
 * @use_bold: Whether @bold can be taken into account.
 * @use_strikethrough: Whether @strikethrough can be taken into account.
 *
 * The data of a #GtkSourceStyle object.
 *
 * Before using the value of a certain field, check the boolean value of the
 * corresponding "use_" field.
 *
 * Since: 300.0
 */
struct _GtkSourceStyleData
{
	GdkRGBA foreground_color;
	GdkRGBA background_color;
	GdkRGBA underline_color;
	GdkRGBA paragraph_background_color;
	gdouble scale;
	PangoUnderline underline;
	guint italic : 1;
	guint bold : 1;
	guint strikethrough : 1;

	guint use_foreground_color : 1;
	guint use_background_color : 1;
	guint use_underline_color : 1;
	guint use_paragraph_background_color : 1;
	guint use_scale : 1;
	guint use_underline : 1;
	guint use_italic : 1;
	guint use_bold : 1;
	guint use_strikethrough : 1;
};

G_MODULE_EXPORT
GType			gtk_source_style_get_type		(void);

G_MODULE_EXPORT
gboolean		gtk_source_style_get_scale		(GtkSourceStyle *style,
								 gdouble        *scale);

G_MODULE_EXPORT
GtkSourceStyle *	gtk_source_style_copy			(const GtkSourceStyle *style);

G_MODULE_EXPORT
void			gtk_source_style_apply			(const GtkSourceStyle *style,
								 GtkTextTag           *tag);

G_END_DECLS

#endif /* GTK_SOURCE_STYLE_H */
