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

#ifndef GTK_SOURCE_STYLE_SCHEME_PRIVATE_H
#define GTK_SOURCE_STYLE_SCHEME_PRIVATE_H

#include "gtksourcestyleschemecss.h"

G_BEGIN_DECLS

G_GNUC_INTERNAL
GtkSourceStyleSchemeCss *
		_gtk_source_style_scheme_get_style_scheme_css	(GtkSourceStyleScheme *scheme);

G_END_DECLS

#endif /* GTK_SOURCE_STYLE_SCHEME_PRIVATE_H */
