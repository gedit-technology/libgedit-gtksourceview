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

#ifndef GTK_SOURCE_STYLE_PRIVATE_H
#define GTK_SOURCE_STYLE_PRIVATE_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

enum
{
	GTK_SOURCE_STYLE_USE_LINE_BACKGROUND = 1 << 0,
	GTK_SOURCE_STYLE_USE_BACKGROUND      = 1 << 1,
	GTK_SOURCE_STYLE_USE_FOREGROUND      = 1 << 2,
	GTK_SOURCE_STYLE_USE_ITALIC          = 1 << 3,
	GTK_SOURCE_STYLE_USE_BOLD            = 1 << 4,
	GTK_SOURCE_STYLE_USE_UNDERLINE       = 1 << 5,
	GTK_SOURCE_STYLE_USE_STRIKETHROUGH   = 1 << 6,
	GTK_SOURCE_STYLE_USE_SCALE           = 1 << 7,
	GTK_SOURCE_STYLE_USE_UNDERLINE_COLOR = 1 << 8
};

struct _GtkSourceStyle
{
	GObject parent;

	/* These fields are strings interned with g_intern_string(), so we don't
	 * need to copy/free them.
	 */
	const gchar *foreground;
	const gchar *background;
	const gchar *underline_color;
	const gchar *scale;
	const gchar *line_background;

	PangoUnderline underline;

	guint italic : 1;
	guint bold : 1;
	guint strikethrough : 1;

	guint mask : 12;
};

G_END_DECLS

#endif /* GTK_SOURCE_STYLE_PRIVATE_H */
