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

#include "gtksourcestyleschemeparser.h"
#include <pango/pango.h>

/*
 * _gtk_source_style_scheme_parser_parse_scale:
 * @scale_str: the input value as a string.
 * @scale_factor: (out) (optional): the output value as a number.
 *
 * This function parses @scale_str and - if successful - writes the output value
 * to @scale_factor. @scale_factor is then suitable for the #GtkTextTag's
 * #GtkTextTag:scale property.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 */
gboolean
_gtk_source_style_scheme_parser_parse_scale (const gchar *scale_str,
					     gdouble     *scale_factor)
{
	gdouble val;
	gint i;
	struct { const gchar *name; gdouble value; } pango_scale_table[] = {
		{ "xx-small",	PANGO_SCALE_XX_SMALL },
		{ "x-small",	PANGO_SCALE_X_SMALL },
		{ "small",	PANGO_SCALE_SMALL },
		{ "medium",	PANGO_SCALE_MEDIUM },
		{ "large",	PANGO_SCALE_LARGE },
		{ "x-large",	PANGO_SCALE_X_LARGE },
		{ "xx-large",	PANGO_SCALE_XX_LARGE },
		{ NULL,		0.0 }
	};

	if (scale_factor != NULL)
	{
		/* Ensure to set a value, as a safety net. */
		*scale_factor = 1.0;
	}

	g_return_val_if_fail (scale_str != NULL, FALSE);

	for (i = 0; pango_scale_table[i].name != NULL; i++)
	{
		if (g_str_equal (scale_str, pango_scale_table[i].name))
		{
			if (scale_factor != NULL)
			{
				*scale_factor = pango_scale_table[i].value;
			}
			return TRUE;
		}
	}

	/* Ideally: have a g_ascii_string_to_signed() equivalent for double and
	 * use it. To check that scale_str contains *only* a valid floating
	 * point number.
	 */
	val = g_ascii_strtod (scale_str, NULL);
	if (val > 0.0)
	{
		if (scale_factor != NULL)
		{
			*scale_factor = val;
		}
		return TRUE;
	}

	return FALSE;
}
