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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtksourcestyle.h"
#include "gtksourcestyle-private.h"

/**
 * SECTION:style
 * @Short_description: Represents a style
 * @Title: GtkSourceStyle
 *
 * The #GtkSourceStyle structure is used to describe text attributes which are
 * set when the given style is used.
 */

struct _GtkSourceStyleClass
{
	GObjectClass parent_class;
};

enum
{
	PROP_0,
	PROP_BOLD,
	PROP_BOLD_SET,
	PROP_PANGO_UNDERLINE,
	PROP_UNDERLINE_SET,
	PROP_UNDERLINE_COLOR,
	PROP_UNDERLINE_COLOR_SET,
	PROP_STRIKETHROUGH,
	PROP_STRIKETHROUGH_SET,

	/* FIXME: Not documented in style-scheme-file-format.html. Is it normal? */
	PROP_LINE_BACKGROUND,
	PROP_LINE_BACKGROUND_SET,

	N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES];

G_DEFINE_TYPE (GtkSourceStyle, gtk_source_style, G_TYPE_OBJECT)

static void
gtk_source_style_get_property (GObject      *object,
			       guint         prop_id,
			       GValue       *value,
			       GParamSpec   *pspec)
{
	GtkSourceStyle *style = GTK_SOURCE_STYLE (object);

	switch (prop_id)
	{
		case PROP_BOLD:
			g_value_set_boolean (value, style->bold);
			break;

		case PROP_BOLD_SET:
			g_value_set_boolean (value, style->mask & GTK_SOURCE_STYLE_USE_BOLD);
			break;

		case PROP_PANGO_UNDERLINE:
			g_value_set_enum (value, style->underline);
			break;

		case PROP_UNDERLINE_SET:
			g_value_set_boolean (value, style->mask & GTK_SOURCE_STYLE_USE_UNDERLINE);
			break;

		case PROP_UNDERLINE_COLOR:
			g_value_set_string (value, style->underline_color);
			break;

		case PROP_UNDERLINE_COLOR_SET:
			g_value_set_boolean (value, style->mask & GTK_SOURCE_STYLE_USE_UNDERLINE_COLOR);
			break;

		case PROP_STRIKETHROUGH:
			g_value_set_boolean (value, style->strikethrough);
			break;

		case PROP_STRIKETHROUGH_SET:
			g_value_set_boolean (value, style->mask & GTK_SOURCE_STYLE_USE_STRIKETHROUGH);
			break;

		case PROP_LINE_BACKGROUND:
			g_value_set_string (value, style->line_background);
			break;

		case PROP_LINE_BACKGROUND_SET:
			g_value_set_boolean (value, style->mask & GTK_SOURCE_STYLE_USE_LINE_BACKGROUND);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gtk_source_style_set_property (GObject      *object,
			       guint         prop_id,
			       const GValue *value,
			       GParamSpec   *pspec)
{
	GtkSourceStyle *style = GTK_SOURCE_STYLE (object);
	const gchar *string;

	switch (prop_id)
	{
		case PROP_BOLD:
			style->bold = g_value_get_boolean (value) != FALSE;
			style->mask |= GTK_SOURCE_STYLE_USE_BOLD;
			break;

		case PROP_BOLD_SET:
			if (g_value_get_boolean (value))
			{
				style->mask |= GTK_SOURCE_STYLE_USE_BOLD;
			}
			else
			{
				style->mask &= ~GTK_SOURCE_STYLE_USE_BOLD;
			}
			break;

		case PROP_PANGO_UNDERLINE:
			style->underline = g_value_get_enum (value);
			style->mask |= GTK_SOURCE_STYLE_USE_UNDERLINE;
			break;

		case PROP_UNDERLINE_SET:
			if (g_value_get_boolean (value))
			{
				style->mask |= GTK_SOURCE_STYLE_USE_UNDERLINE;
			}
			else
			{
				style->mask &= ~GTK_SOURCE_STYLE_USE_UNDERLINE;
			}
			break;

		case PROP_UNDERLINE_COLOR:
			string = g_value_get_string (value);
			if (string != NULL)
			{
				style->underline_color = g_intern_string (string);
				style->mask |= GTK_SOURCE_STYLE_USE_UNDERLINE_COLOR;
			}
			else
			{
				style->underline_color = NULL;
				style->mask &= ~GTK_SOURCE_STYLE_USE_UNDERLINE_COLOR;
			}
			break;

		case PROP_UNDERLINE_COLOR_SET:
			if (g_value_get_boolean (value))
			{
				style->mask |= GTK_SOURCE_STYLE_USE_UNDERLINE_COLOR;
			}
			else
			{
				style->mask &= ~GTK_SOURCE_STYLE_USE_UNDERLINE_COLOR;
			}
			break;

		case PROP_STRIKETHROUGH:
			style->strikethrough = g_value_get_boolean (value) != FALSE;
			style->mask |= GTK_SOURCE_STYLE_USE_STRIKETHROUGH;
			break;

		case PROP_STRIKETHROUGH_SET:
			if (g_value_get_boolean (value))
			{
				style->mask |= GTK_SOURCE_STYLE_USE_STRIKETHROUGH;
			}
			else
			{
				style->mask &= ~GTK_SOURCE_STYLE_USE_STRIKETHROUGH;
			}
			break;

		case PROP_LINE_BACKGROUND:
			string = g_value_get_string (value);
			if (string != NULL)
			{
				style->line_background = g_intern_string (string);
				style->mask |= GTK_SOURCE_STYLE_USE_LINE_BACKGROUND;
			}
			else
			{
				style->line_background = NULL;
				style->mask &= ~GTK_SOURCE_STYLE_USE_LINE_BACKGROUND;
			}
			break;

		case PROP_LINE_BACKGROUND_SET:
			if (g_value_get_boolean (value))
			{
				style->mask |= GTK_SOURCE_STYLE_USE_LINE_BACKGROUND;
			}
			else
			{
				style->mask &= ~GTK_SOURCE_STYLE_USE_LINE_BACKGROUND;
			}
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gtk_source_style_class_init (GtkSourceStyleClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->get_property = gtk_source_style_get_property;
	object_class->set_property = gtk_source_style_set_property;

	/* All properties are CONSTRUCT_ONLY so we can safely return references
	 * from style_scheme_get_style(). But style scheme is of course cheating
	 * and sets everything after construction (but nobody can notice it).
	 */

	/**
	 * GtkSourceStyle:bold:
	 *
	 * Bold.
	 */
	properties[PROP_BOLD] =
		g_param_spec_boolean ("bold",
				      "bold",
				      "",
				      FALSE,
				      G_PARAM_READWRITE |
				      G_PARAM_CONSTRUCT_ONLY |
				      G_PARAM_STATIC_STRINGS);

	/**
	 * GtkSourceStyle:bold-set:
	 *
	 * Whether #GtkSourceStyle:bold is set.
	 */
	properties[PROP_BOLD_SET] =
		g_param_spec_boolean ("bold-set",
				      "bold-set",
				      "",
				      FALSE,
				      G_PARAM_READWRITE |
				      G_PARAM_CONSTRUCT_ONLY |
				      G_PARAM_STATIC_STRINGS);

	/**
	 * GtkSourceStyle:pango-underline:
	 *
	 * Pango underline.
	 */
	properties[PROP_PANGO_UNDERLINE] =
		g_param_spec_enum ("pango-underline",
				   "pango-underline",
				   "",
				   PANGO_TYPE_UNDERLINE,
				   PANGO_UNDERLINE_NONE,
				   G_PARAM_READWRITE |
				   G_PARAM_CONSTRUCT_ONLY |
				   G_PARAM_STATIC_STRINGS);

	/**
	 * GtkSourceStyle:underline-set:
	 *
	 * Whether #GtkSourceStyle:pango-underline is set.
	 */
	properties[PROP_UNDERLINE_SET] =
		g_param_spec_boolean ("underline-set",
				      "underline-set",
				      "",
				      FALSE,
				      G_PARAM_READWRITE |
				      G_PARAM_CONSTRUCT_ONLY |
				      G_PARAM_STATIC_STRINGS);

	/**
	 * GtkSourceStyle:underline-color:
	 *
	 * Underline color.
	 */
	properties[PROP_UNDERLINE_COLOR] =
		g_param_spec_string ("underline-color",
				     "underline-color",
				     "",
				     NULL,
				     G_PARAM_READWRITE |
				     G_PARAM_CONSTRUCT_ONLY |
				     G_PARAM_STATIC_STRINGS);

	/**
	 * GtkSourceStyle:underline-color-set:
	 *
	 * Whether #GtkSourceStyle:underline-color is set.
	 */
	properties[PROP_UNDERLINE_COLOR_SET] =
		g_param_spec_boolean ("underline-color-set",
				      "underline-color-set",
				      "",
				      FALSE,
				      G_PARAM_READWRITE |
				      G_PARAM_CONSTRUCT_ONLY |
				      G_PARAM_STATIC_STRINGS);

	/**
	 * GtkSourceStyle:strikethrough:
	 *
	 * Strikethrough.
	 */
	properties[PROP_STRIKETHROUGH] =
		g_param_spec_boolean ("strikethrough",
				      "strikethrough",
				      "",
				      FALSE,
				      G_PARAM_READWRITE |
				      G_PARAM_CONSTRUCT_ONLY |
				      G_PARAM_STATIC_STRINGS);

	/**
	 * GtkSourceStyle:strikethrough-set:
	 *
	 * Whether #GtkSourceStyle:strikethrough is set.
	 */
	properties[PROP_STRIKETHROUGH_SET] =
		g_param_spec_boolean ("strikethrough-set",
				      "strikethrough-set",
				      "",
				      FALSE,
				      G_PARAM_READWRITE |
				      G_PARAM_CONSTRUCT_ONLY |
				      G_PARAM_STATIC_STRINGS);

	/**
	 * GtkSourceStyle:line-background:
	 *
	 * Line background color.
	 */
	properties[PROP_LINE_BACKGROUND] =
		g_param_spec_string ("line-background",
				     "line-background",
				     "",
				     NULL,
				     G_PARAM_READWRITE |
				     G_PARAM_CONSTRUCT_ONLY |
				     G_PARAM_STATIC_STRINGS);

	/**
	 * GtkSourceStyle:line-background-set:
	 *
	 * Whether #GtkSourceStyle:line-background is set.
	 */
	properties[PROP_LINE_BACKGROUND_SET] =
		g_param_spec_boolean ("line-background-set",
				      "line-background-set",
				      "",
				      FALSE,
				      G_PARAM_READWRITE |
				      G_PARAM_CONSTRUCT_ONLY |
				      G_PARAM_STATIC_STRINGS);

	g_object_class_install_properties (object_class, N_PROPERTIES, properties);
}

static void
gtk_source_style_init (GtkSourceStyle *style)
{
}

/**
 * gtk_source_style_get_data: (skip)
 * @style: a #GtkSourceStyle.
 *
 * Returns: (transfer full): a new #GtkSourceStyleData. Free with g_free() when
 *   no longer needed.
 * Since: 300.0
 */
GtkSourceStyleData *
gtk_source_style_get_data (GtkSourceStyle *style)
{
	GtkSourceStyleData *data;

	g_return_val_if_fail (GTK_SOURCE_IS_STYLE (style), NULL);

	data = g_new0 (GtkSourceStyleData, 1);

	if (style->mask & GTK_SOURCE_STYLE_USE_FOREGROUND)
	{
		data->use_foreground_color = gdk_rgba_parse (&data->foreground_color, style->foreground);
		g_warn_if_fail (data->use_foreground_color);
	}
	if (style->mask & GTK_SOURCE_STYLE_USE_BACKGROUND)
	{
		data->use_background_color = gdk_rgba_parse (&data->background_color, style->background);
		g_warn_if_fail (data->use_background_color);
	}
	if (style->mask & GTK_SOURCE_STYLE_USE_UNDERLINE_COLOR)
	{
		data->use_underline_color = gdk_rgba_parse (&data->underline_color, style->underline_color);
		g_warn_if_fail (data->use_underline_color);
	}
	if (style->mask & GTK_SOURCE_STYLE_USE_LINE_BACKGROUND)
	{
		data->use_paragraph_background_color = gdk_rgba_parse (&data->paragraph_background_color, style->line_background);
		g_warn_if_fail (data->use_paragraph_background_color);
	}

	data->scale = style->scale;
	data->use_scale = style->use_scale;

	data->underline = style->underline;
	data->use_underline = (style->mask & GTK_SOURCE_STYLE_USE_UNDERLINE) != 0;

	data->italic = style->italic;
	data->use_italic = (style->mask & GTK_SOURCE_STYLE_USE_ITALIC) != 0;

	data->bold = style->bold;
	data->use_bold = (style->mask & GTK_SOURCE_STYLE_USE_BOLD) != 0;

	data->strikethrough = style->strikethrough;
	data->use_strikethrough = (style->mask & GTK_SOURCE_STYLE_USE_STRIKETHROUGH) != 0;

	return data;
}

/**
 * gtk_source_style_copy:
 * @style: a #GtkSourceStyle structure to copy.
 *
 * Creates a copy of @style, that is a new #GtkSourceStyle instance which
 * has the same attributes set.
 *
 * Returns: (transfer full): copy of @style, call g_object_unref()
 * when you are done with it.
 *
 * Since: 2.0
 */
GtkSourceStyle *
gtk_source_style_copy (const GtkSourceStyle *style)
{
	GtkSourceStyle *copy;

	g_return_val_if_fail (GTK_SOURCE_IS_STYLE (style), NULL);

	copy = g_object_new (GTK_SOURCE_TYPE_STYLE, NULL);

	copy->foreground = style->foreground;
	copy->background = style->background;
	copy->underline_color = style->underline_color;
	copy->line_background = style->line_background;
	copy->scale = style->scale;
	copy->underline = style->underline;
	copy->italic = style->italic;
	copy->bold = style->bold;
	copy->strikethrough = style->strikethrough;
	copy->mask = style->mask;
	copy->use_scale = style->use_scale;

	return copy;
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
 * If @style is %NULL, the related *-set properties of #GtkTextTag are set to
 * %FALSE.
 *
 * Since: 3.22
 */
void
gtk_source_style_apply (GtkSourceStyle *style,
			GtkTextTag     *tag)
{
	g_return_if_fail (style == NULL || GTK_SOURCE_IS_STYLE (style));
	g_return_if_fail (GTK_IS_TEXT_TAG (tag));

	if (style != NULL)
	{
		GtkSourceStyleData *data = gtk_source_style_get_data (style);

		g_object_freeze_notify (G_OBJECT (tag));

		if (data->use_foreground_color)
		{
			g_object_set (tag, "foreground-rgba", &data->foreground_color, NULL);
		}
		else
		{
			g_object_set (tag, "foreground-set", FALSE, NULL);
		}

		if (data->use_background_color)
		{
			g_object_set (tag, "background-rgba", &data->background_color, NULL);
		}
		else
		{
			g_object_set (tag, "background-set", FALSE, NULL);
		}

		if (data->use_italic)
		{
			g_object_set (tag, "style", data->italic ? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL, NULL);
		}
		else
		{
			g_object_set (tag, "style-set", FALSE, NULL);
		}

		if (data->use_bold)
		{
			g_object_set (tag, "weight", data->bold ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL, NULL);
		}
		else
		{
			g_object_set (tag, "weight-set", FALSE, NULL);
		}

		if (data->use_underline)
		{
			g_object_set (tag, "underline", data->underline, NULL);
		}
		else
		{
			g_object_set (tag, "underline-set", FALSE, NULL);
		}

		if (data->use_underline_color)
		{
			g_object_set (tag, "underline-rgba", &data->underline_color, NULL);
		}
		else
		{
			g_object_set (tag, "underline-rgba-set", FALSE, NULL);
		}

		if (data->use_strikethrough)
		{
			g_object_set (tag, "strikethrough", data->strikethrough, NULL);
		}
		else
		{
			g_object_set (tag, "strikethrough-set", FALSE, NULL);
		}

		if (data->use_scale)
		{
			g_object_set (tag, "scale", data->scale, NULL);
		}
		else
		{
			g_object_set (tag, "scale-set", FALSE, NULL);
		}

		if (data->use_paragraph_background_color)
		{
			g_object_set (tag, "paragraph-background-rgba", &data->paragraph_background_color, NULL);
		}
		else
		{
			g_object_set (tag, "paragraph-background-set", FALSE, NULL);
		}

		g_object_thaw_notify (G_OBJECT (tag));

		g_free (data);
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
			      "paragraph-background-set", FALSE,
			      NULL);
	}
}
