/*
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2003 - Paolo Maggi <paolo.maggi@polito.it>
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

#include "gtksourcestyle.h"

/**
 * SECTION:style
 * @Title: GtkSourceStyle
 * @Short_description: A style
 *
 * The #GtkSourceStyle structure contains text attributes.
 */

/* API design:
 *
 * In a past implementation, GtkSourceStyle was a GObject with a lot of
 * properties to get each individual attribute (the properties were read-write
 * and construct-only). This was language-bindings friendly, but a bit too
 * "heavy" as a solution.
 *
 * Thus it is now a boxed type.
 */

G_DEFINE_BOXED_TYPE (GtkSourceStyle, gtk_source_style,
		     gtk_source_style_ref,
		     gtk_source_style_unref)

/**
 * gtk_source_style_new:
 *
 * Returns: a new #GtkSourceStyle with a reference count of one.
 * Since: 300.0
 */
GtkSourceStyle *
gtk_source_style_new (void)
{
	GtkSourceStyle *style;

	style = g_new0 (GtkSourceStyle, 1);
	g_ref_count_init (&style->ref_count);

	return style;
}

/**
 * gtk_source_style_ref:
 * @style: a #GtkSourceStyle.
 *
 * Increases the reference count of @style by one.
 *
 * Returns: (transfer full): the passed in @style.
 * Since: 300.0
 */
GtkSourceStyle *
gtk_source_style_ref (GtkSourceStyle *style)
{
	g_return_val_if_fail (style != NULL, NULL);

	g_ref_count_inc (&style->ref_count);

	return style;
}

/**
 * gtk_source_style_unref:
 * @style: a #GtkSourceStyle.
 *
 * Decreases the reference count of @style by one. If the reference count drops
 * to 0, @style is freed.
 *
 * Since: 300.0
 */
void
gtk_source_style_unref (GtkSourceStyle *style)
{
	g_return_if_fail (style != NULL);

	if (g_ref_count_dec (&style->ref_count))
	{
		g_free (style);
	}
}

/**
 * gtk_source_style_apply:
 * @style: (nullable): a #GtkSourceStyle to apply, or %NULL.
 * @tag: a #GtkTextTag to apply styles to.
 *
 * This function modifies the #GtkTextTag properties that are related to the
 * #GtkSourceStyle attributes. Other #GtkTextTag properties are left untouched.
 *
 * If @style is non-%NULL, applies @style to @tag.
 *
 * If @style is %NULL, the related `*-set` properties of #GtkTextTag are set to
 * %FALSE.
 *
 * Since: 3.22
 */
void
gtk_source_style_apply (GtkSourceStyle *style,
			GtkTextTag     *tag)
{
	g_return_if_fail (GTK_IS_TEXT_TAG (tag));

	if (style != NULL)
	{
		g_object_freeze_notify (G_OBJECT (tag));

		if (style->use_foreground_color)
		{
			g_object_set (tag, "foreground-rgba", &style->foreground_color, NULL);
		}
		else
		{
			g_object_set (tag, "foreground-set", FALSE, NULL);
		}

		if (style->use_background_color)
		{
			g_object_set (tag, "background-rgba", &style->background_color, NULL);
		}
		else
		{
			g_object_set (tag, "background-set", FALSE, NULL);
		}

		if (style->use_italic)
		{
			g_object_set (tag, "style", style->italic ? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL, NULL);
		}
		else
		{
			g_object_set (tag, "style-set", FALSE, NULL);
		}

		if (style->use_bold)
		{
			g_object_set (tag, "weight", style->bold ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL, NULL);
		}
		else
		{
			g_object_set (tag, "weight-set", FALSE, NULL);
		}

		if (style->use_underline)
		{
			g_object_set (tag, "underline", style->underline, NULL);
		}
		else
		{
			g_object_set (tag, "underline-set", FALSE, NULL);
		}

		if (style->use_underline_color)
		{
			g_object_set (tag, "underline-rgba", &style->underline_color, NULL);
		}
		else
		{
			g_object_set (tag, "underline-rgba-set", FALSE, NULL);
		}

		if (style->use_strikethrough)
		{
			g_object_set (tag, "strikethrough", style->strikethrough, NULL);
		}
		else
		{
			g_object_set (tag, "strikethrough-set", FALSE, NULL);
		}

		if (style->use_scale)
		{
			g_object_set (tag, "scale", style->scale, NULL);
		}
		else
		{
			g_object_set (tag, "scale-set", FALSE, NULL);
		}

		g_object_thaw_notify (G_OBJECT (tag));
	}
	else
	{
		g_object_set (tag,
			      "foreground-set", FALSE,
			      "background-set", FALSE,
			      "style-set", FALSE,
			      "weight-set", FALSE,
			      "underline-set", FALSE,
			      "underline-rgba-set", FALSE,
			      "strikethrough-set", FALSE,
			      "scale-set", FALSE,
			      NULL);
	}
}
