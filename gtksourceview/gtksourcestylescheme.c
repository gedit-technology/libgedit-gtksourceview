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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtksourcestylescheme.h"
#include "gtksourcestylescheme-private.h"
#include <libxml/parser.h>
#include <string.h>
#include <glib/gi18n-lib.h>
#include "gtksourcestyle-private.h"
#include "gtksourcestyleschemeparser.h"

/**
 * SECTION:stylescheme
 * @Short_description: Controls the appearance of GtkSourceView
 * @Title: GtkSourceStyleScheme
 *
 * #GtkSourceStyleScheme contains all the text styles to be used in
 * #GtkSourceView and #GtkSourceBuffer. For instance, it contains text styles
 * for syntax highlighting, it may contain foreground and background color for
 * non-highlighted text, color for the line numbers, current line highlighting,
 * bracket matching, etc.
 *
 * Style schemes are stored in XML files. The format of a scheme file is
 * documented in the [style scheme reference][style-reference].
 *
 * The two style schemes with IDs "classic" and "tango" follow more closely the
 * GTK theme (for example for the background color).
 */

#define STYLE_SCHEME_VERSION "1.0"

struct _GtkSourceStyleSchemePrivate
{
	gchar *id;
	gchar *name;
	GPtrArray *authors;
	gchar *description;
	gchar *filename;

	GtkSourceStyleScheme *parent;
	gchar *parent_id;

	GHashTable *defined_styles;
	GHashTable *style_cache;
	GHashTable *named_colors;

	GtkSourceStyleSchemeCss *scheme_css;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtkSourceStyleScheme, gtk_source_style_scheme, G_TYPE_OBJECT)

static void
gtk_source_style_scheme_dispose (GObject *object)
{
	GtkSourceStyleScheme *scheme = GTK_SOURCE_STYLE_SCHEME (object);

	if (scheme->priv->named_colors != NULL)
	{
		g_hash_table_unref (scheme->priv->named_colors);
		scheme->priv->named_colors = NULL;
	}

	if (scheme->priv->style_cache != NULL)
	{
		g_hash_table_unref (scheme->priv->style_cache);
		scheme->priv->style_cache = NULL;
	}

	if (scheme->priv->defined_styles != NULL)
	{
		g_hash_table_unref (scheme->priv->defined_styles);
		scheme->priv->defined_styles = NULL;
	}

	g_clear_object (&scheme->priv->parent);
	g_clear_object (&scheme->priv->scheme_css);

	G_OBJECT_CLASS (gtk_source_style_scheme_parent_class)->dispose (object);
}

static void
gtk_source_style_scheme_finalize (GObject *object)
{
	GtkSourceStyleScheme *scheme = GTK_SOURCE_STYLE_SCHEME (object);

	if (scheme->priv->authors != NULL)
	{
		g_ptr_array_free (scheme->priv->authors, TRUE);
	}

	g_free (scheme->priv->filename);
	g_free (scheme->priv->description);
	g_free (scheme->priv->id);
	g_free (scheme->priv->name);
	g_free (scheme->priv->parent_id);

	G_OBJECT_CLASS (gtk_source_style_scheme_parent_class)->finalize (object);
}

static void
gtk_source_style_scheme_class_init (GtkSourceStyleSchemeClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gtk_source_style_scheme_dispose;
	object_class->finalize = gtk_source_style_scheme_finalize;
}

static void
unref_if_not_null (gpointer object)
{
	if (object != NULL)
	{
		g_object_unref (object);
	}
}

static void
gtk_source_style_scheme_init (GtkSourceStyleScheme *scheme)
{
	scheme->priv = gtk_source_style_scheme_get_instance_private (scheme);

	scheme->priv->defined_styles = g_hash_table_new_full (g_str_hash, g_str_equal,
							      g_free, g_object_unref);

	scheme->priv->style_cache = g_hash_table_new_full (g_str_hash, g_str_equal,
							   g_free, unref_if_not_null);

	scheme->priv->named_colors = g_hash_table_new_full (g_str_hash, g_str_equal,
							    g_free, g_free);
}

/**
 * gtk_source_style_scheme_get_id:
 * @scheme: a #GtkSourceStyleScheme.
 *
 * Returns: the @scheme ID, a unique string used to identify the style scheme in
 *   a #GtkSourceStyleSchemeManager.
 * Since: 2.0
 */
const gchar *
gtk_source_style_scheme_get_id (GtkSourceStyleScheme *scheme)
{
	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME (scheme), NULL);

	return scheme->priv->id;
}

/**
 * gtk_source_style_scheme_get_name:
 * @scheme: a #GtkSourceStyleScheme.
 *
 * Returns: the @scheme name, a human-readable (translated) string to present to
 *   the user.
 * Since: 2.0
 */
const gchar *
gtk_source_style_scheme_get_name (GtkSourceStyleScheme *scheme)
{
	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME (scheme), NULL);

	return scheme->priv->name;
}

/**
 * gtk_source_style_scheme_get_description:
 * @scheme: a #GtkSourceStyleScheme.
 *
 * Returns: (nullable): the @scheme description, a human-readable (translated)
 *   string to present to the user.
 * Since: 2.0
 */
const gchar *
gtk_source_style_scheme_get_description (GtkSourceStyleScheme *scheme)
{
	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME (scheme), NULL);

	return scheme->priv->description;
}

/**
 * gtk_source_style_scheme_get_authors:
 * @scheme: a #GtkSourceStyleScheme.
 *
 * Returns: (nullable) (array zero-terminated=1) (transfer none): a
 *   %NULL-terminated array containing the @scheme authors, or %NULL if no
 *   author is specified by the style scheme.
 * Since: 2.0
 */
const gchar * const *
gtk_source_style_scheme_get_authors (GtkSourceStyleScheme *scheme)
{
	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME (scheme), NULL);

	if (scheme->priv->authors == NULL)
	{
		return NULL;
	}

	return (const gchar * const *)scheme->priv->authors->pdata;
}

/**
 * gtk_source_style_scheme_get_filename:
 * @scheme: a #GtkSourceStyleScheme.
 *
 * Returns: (nullable): @scheme file name if the scheme was created parsing a
 *   style scheme file, or %NULL in the other cases.
 * Since: 2.0
 */
const gchar *
gtk_source_style_scheme_get_filename (GtkSourceStyleScheme *scheme)
{
	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME (scheme), NULL);

	return scheme->priv->filename;
}

/*
 * get_color_by_name:
 * @scheme: a #GtkSourceStyleScheme.
 * @name: color name to find.
 *
 * Returned value is actual color string suitable for
 * _gtk_source_style_scheme_parser_parse_final_color(). It may be @name so copy
 * it or something, if you need it to stay around.
 *
 * Returns: color which corresponds to @name in the @scheme.
 */
static const gchar *
get_color_by_name (GtkSourceStyleScheme *scheme,
		   const gchar          *name)
{
	const gchar *color = NULL;

	g_return_val_if_fail (name != NULL, NULL);

	if (name[0] == '#')
	{
		GdkRGBA dummy_rgba;

		if (_gtk_source_style_scheme_parser_parse_final_color (name, &dummy_rgba))
		{
			color = name;
		}
		else
		{
			g_warning ("could not parse color '%s'", name);
		}
	}
	else
	{
		color = g_hash_table_lookup (scheme->priv->named_colors, name);

		if (color == NULL && scheme->priv->parent != NULL)
		{
			color = get_color_by_name (scheme->priv->parent, name);
		}

		if (color == NULL)
		{
			g_warning ("no color named '%s'", name);
		}
	}

	return color;
}

static void
fix_style_color_for_attribute (GtkSourceStyleScheme  *scheme,
			       GtkSourceStyle        *style,
			       guint                  attr_mask,
			       const gchar          **attr_value)
{
	if (style->mask & attr_mask)
	{
		const gchar *color = get_color_by_name (scheme, *attr_value);

		if (color != NULL)
		{
			*attr_value = g_intern_string (color);
		}
		else
		{
			style->mask &= ~attr_mask;
		}
	}
}

static GtkSourceStyle *
fix_style_colors (GtkSourceStyleScheme *scheme,
		  GtkSourceStyle       *real_style)
{
	GtkSourceStyle *style = gtk_source_style_copy (real_style);

	fix_style_color_for_attribute (scheme, style, GTK_SOURCE_STYLE_USE_BACKGROUND, &style->background);
	fix_style_color_for_attribute (scheme, style, GTK_SOURCE_STYLE_USE_FOREGROUND, &style->foreground);
	fix_style_color_for_attribute (scheme, style, GTK_SOURCE_STYLE_USE_LINE_BACKGROUND, &style->line_background);
	fix_style_color_for_attribute (scheme, style, GTK_SOURCE_STYLE_USE_UNDERLINE_COLOR, &style->underline_color);

	return style;
}

/**
 * gtk_source_style_scheme_get_style:
 * @scheme: a #GtkSourceStyleScheme.
 * @style_id: ID of the style to retrieve.
 *
 * Returns: (nullable) (transfer none): the #GtkSourceStyle which corresponds to
 *   @style_id in the @scheme, or %NULL if not found.
 * Since: 2.0
 */
/* It's a little weird because we have named colors: styles loaded from a
 * scheme file can have "#red" or "blue". "#red" is a "final" color, while
 * "blue" refers to a <color> tag. All final colors have # as prefix, and are
 * suitable for _gtk_source_style_scheme_parser_parse_final_color(), to get a
 * GdkRGBA.
 *
 * We need to preserve what we got from the file, because it's possible for a
 * child scheme to redefine colors, so we can't translate colors when loading
 * the scheme.
 *
 * So, the defined_styles hash table has original color values; styles returned
 * with get_style() have final colors.
 */
GtkSourceStyle *
gtk_source_style_scheme_get_style (GtkSourceStyleScheme *scheme,
				   const gchar          *style_id)
{
	GtkSourceStyle *style = NULL;
	GtkSourceStyle *real_style;

	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME (scheme), NULL);
	g_return_val_if_fail (style_id != NULL, NULL);

	if (g_hash_table_lookup_extended (scheme->priv->style_cache,
					  style_id,
					  NULL,
					  (gpointer)&style))
	{
		return style;
	}

	real_style = g_hash_table_lookup (scheme->priv->defined_styles, style_id);

	if (real_style == NULL)
	{
		if (scheme->priv->parent != NULL)
		{
			style = gtk_source_style_scheme_get_style (scheme->priv->parent,
								   style_id);
		}
		if (style != NULL)
		{
			g_object_ref (style);
		}
	}
	else
	{
		style = fix_style_colors (scheme, real_style);
	}

	g_hash_table_insert (scheme->priv->style_cache,
			     g_strdup (style_id),
			     style);

	return style;
}

/* --- PARSER ---------------------------------------------------------------- */

#define ERROR_QUARK (g_quark_from_static_string ("gtk-source-style-scheme-parser-error"))

static gboolean
parse_bool (char *value)
{
	return (g_ascii_strcasecmp (value, "true") == 0 ||
	        g_ascii_strcasecmp (value, "yes") == 0 ||
	        g_ascii_strcasecmp (value, "1") == 0);
}

static void
get_bool (xmlNode    *node,
	  const char *propname,
	  guint      *mask,
	  guint       mask_value,
	  gboolean   *value)
{
	xmlChar *tmp = xmlGetProp (node, BAD_CAST propname);

	if (tmp != NULL)
	{
		*mask |= mask_value;
		*value = parse_bool ((char*) tmp);
	}

	xmlFree (tmp);
}

static gboolean
parse_style (GtkSourceStyleScheme *scheme,
	     xmlNode              *node,
	     gchar               **style_name_p,
	     GtkSourceStyle      **style_p,
	     GError              **error)
{
	GtkSourceStyle *use_style = NULL;
	GtkSourceStyle *result = NULL;
	xmlChar *fg = NULL;
	xmlChar *bg = NULL;
	xmlChar *line_bg = NULL;
	gchar *style_name = NULL;
	guint mask = 0;
	gboolean bold = FALSE;
	gboolean italic = FALSE;
	gboolean strikethrough = FALSE;
	xmlChar *underline = NULL;
	xmlChar *underline_color = NULL;
	xmlChar *scale = NULL;
	xmlChar *tmp;

	tmp = xmlGetProp (node, BAD_CAST "name");
	if (tmp == NULL)
	{
		g_set_error (error, ERROR_QUARK, 0, "name attribute missing");
		return FALSE;
	}
	style_name = g_strdup ((char*) tmp);
	xmlFree (tmp);

	tmp = xmlGetProp (node, BAD_CAST "use-style");
	if (tmp != NULL)
	{
		use_style = gtk_source_style_scheme_get_style (scheme, (char*) tmp);

		if (use_style == NULL)
		{
			g_set_error (error, ERROR_QUARK, 0,
				     "in style '%s': unknown style '%s'",
				     style_name, tmp);
			g_free (style_name);
			return FALSE;
		}

		g_object_ref (use_style);
	}
	xmlFree (tmp);

	fg = xmlGetProp (node, BAD_CAST "foreground");
	bg = xmlGetProp (node, BAD_CAST "background");
	line_bg = xmlGetProp (node, BAD_CAST "line-background");
	get_bool (node, "italic", &mask, GTK_SOURCE_STYLE_USE_ITALIC, &italic);
	get_bool (node, "bold", &mask, GTK_SOURCE_STYLE_USE_BOLD, &bold);
	get_bool (node, "strikethrough", &mask, GTK_SOURCE_STYLE_USE_STRIKETHROUGH, &strikethrough);
	underline = xmlGetProp (node, BAD_CAST "underline");
	underline_color = xmlGetProp (node, BAD_CAST "underline-color");
	scale = xmlGetProp (node, BAD_CAST "scale");

	if (use_style)
	{
		if (fg != NULL ||
		    bg != NULL ||
		    line_bg != NULL ||
		    mask != 0 ||
		    underline != NULL ||
		    underline_color != NULL ||
		    scale != NULL)
		{
			g_set_error (error, ERROR_QUARK, 0,
				     "in style '%s': style attributes used along with use-style",
				     style_name);
			g_object_unref (use_style);
			g_free (style_name);
			xmlFree (fg);
			xmlFree (bg);
			xmlFree (line_bg);
			xmlFree (underline);
			xmlFree (underline_color);
			xmlFree (scale);
			return FALSE;
		}

		result = use_style;
		use_style = NULL;
	}
	else
	{
		result = g_object_new (GTK_SOURCE_TYPE_STYLE, NULL);

		result->mask = mask;
		result->bold = bold;
		result->italic = italic;
		result->strikethrough = strikethrough;

		if (fg != NULL)
		{
			result->foreground = g_intern_string ((char*) fg);
			result->mask |= GTK_SOURCE_STYLE_USE_FOREGROUND;
		}

		if (bg != NULL)
		{
			result->background = g_intern_string ((char*) bg);
			result->mask |= GTK_SOURCE_STYLE_USE_BACKGROUND;
		}

		if (line_bg != NULL)
		{
			result->line_background = g_intern_string ((char*) line_bg);
			result->mask |= GTK_SOURCE_STYLE_USE_LINE_BACKGROUND;
		}

		if (underline != NULL)
		{
			/* Up until 3.16 underline was a "bool", so for backward
			 * compat we accept underline="true" and map it to "single"
			 */
			if (parse_bool ((char *) underline))
			{
				result->underline = PANGO_UNDERLINE_SINGLE;
				result->mask |= GTK_SOURCE_STYLE_USE_UNDERLINE;
			}
			else
			{
				GEnumClass *enum_class;
				GEnumValue *enum_value;
				gchar *underline_lowercase;

				enum_class = G_ENUM_CLASS (g_type_class_ref (PANGO_TYPE_UNDERLINE));

				underline_lowercase = g_ascii_strdown ((char*) underline, -1);
				enum_value = g_enum_get_value_by_nick (enum_class, underline_lowercase);
				g_free (underline_lowercase);

				if (enum_value != NULL)
				{
					result->underline = enum_value->value;
					result->mask |= GTK_SOURCE_STYLE_USE_UNDERLINE;
				}

				g_type_class_unref (enum_class);
			}
		}

		if (underline_color != NULL)
		{
			result->underline_color = g_intern_string ((char*) underline_color);
			result->mask |= GTK_SOURCE_STYLE_USE_UNDERLINE_COLOR;
		}

		if (scale != NULL)
		{
			gdouble scale_factor = 1.0;

			if (_gtk_source_style_scheme_parser_parse_scale ((char*) scale, &scale_factor))
			{
				result->scale = scale_factor;
				result->use_scale = TRUE;
			}
			else
			{
				g_warning ("Style scheme: failed to parse the scale attribute: '%s' is not a valid value.",
					   (char*) scale);
			}
		}
	}

	*style_p = result;
	*style_name_p = style_name;

	xmlFree (fg);
	xmlFree (bg);
	xmlFree (line_bg);
	xmlFree (underline);
	xmlFree (underline_color);
	xmlFree (scale);

	return TRUE;
}

static gboolean
parse_color (GtkSourceStyleScheme *scheme,
	     xmlNode              *node,
	     GError              **error)
{
	xmlChar *name, *value;
	gboolean result = FALSE;

	name = xmlGetProp (node, BAD_CAST "name");
	value = xmlGetProp (node, BAD_CAST "value");

	if (name == NULL || name[0] == 0)
		g_set_error (error, ERROR_QUARK, 0, "name attribute missing in 'color' tag");
	else if (value == NULL)
		g_set_error (error, ERROR_QUARK, 0, "value attribute missing in 'color' tag");
	else if (value[0] != '#' || value[1] == 0)
		g_set_error (error, ERROR_QUARK, 0, "value in 'color' tag is not of the form '#RGB' or '#name'");
	else if (g_hash_table_lookup (scheme->priv->named_colors, name) != NULL)
		g_set_error (error, ERROR_QUARK, 0, "duplicated color '%s'", name);
	else
		result = TRUE;

	if (result)
		g_hash_table_insert (scheme->priv->named_colors,
				     g_strdup ((char *) name),
				     g_strdup ((char *) value));

	xmlFree (value);
	xmlFree (name);

	return result;
}

static gboolean
parse_style_scheme_child (GtkSourceStyleScheme *scheme,
			  xmlNode              *node,
			  GError              **error)
{
	if (strcmp ((char*) node->name, "style") == 0)
	{
		GtkSourceStyle *style;
		gchar *style_name;

		if (!parse_style (scheme, node, &style_name, &style, error))
			return FALSE;

		g_hash_table_insert (scheme->priv->defined_styles, style_name, style);
	}
	else if (strcmp ((char*) node->name, "color") == 0)
	{
		if (!parse_color (scheme, node, error))
			return FALSE;
	}
	else if (strcmp ((char*) node->name, "author") == 0)
	{
		xmlChar *tmp = xmlNodeGetContent (node);
		if (scheme->priv->authors == NULL)
			scheme->priv->authors = g_ptr_array_new_with_free_func (g_free);

		g_ptr_array_add (scheme->priv->authors, g_strdup ((char*) tmp));

		xmlFree (tmp);
	}
	else if (strcmp ((char*) node->name, "description") == 0)
	{
		xmlChar *tmp = xmlNodeGetContent (node);
		scheme->priv->description = g_strdup ((char*) tmp);
		xmlFree (tmp);
	}
	else if (strcmp ((char*) node->name, "_description") == 0)
	{
		xmlChar *tmp = xmlNodeGetContent (node);
		scheme->priv->description = g_strdup (_((char*) tmp));
		xmlFree (tmp);
	}
	else
	{
		g_set_error (error, ERROR_QUARK, 0, "unknown node '%s'", node->name);
		return FALSE;
	}

	return TRUE;
}

static void
parse_style_scheme_element (GtkSourceStyleScheme *scheme,
			    xmlNode              *scheme_node,
			    GError              **error)
{
	xmlChar *tmp;
	xmlNode *node;

	if (strcmp ((char*) scheme_node->name, "style-scheme") != 0)
	{
		g_set_error (error, ERROR_QUARK, 0,
			     "unexpected element '%s'",
			     (char*) scheme_node->name);
		return;
	}

	tmp = xmlGetProp (scheme_node, BAD_CAST "version");
	if (tmp == NULL)
	{
		g_set_error (error, ERROR_QUARK, 0, "missing 'version' attribute");
		return;
	}
	if (strcmp ((char*) tmp, STYLE_SCHEME_VERSION) != 0)
	{
		g_set_error (error, ERROR_QUARK, 0, "unsupported version '%s'", (char*) tmp);
		xmlFree (tmp);
		return;
	}
	xmlFree (tmp);

	tmp = xmlGetProp (scheme_node, BAD_CAST "id");
	if (tmp == NULL)
	{
		g_set_error (error, ERROR_QUARK, 0, "missing 'id' attribute");
		return;
	}
	scheme->priv->id = g_strdup ((char*) tmp);
	xmlFree (tmp);

	tmp = xmlGetProp (scheme_node, BAD_CAST "_name");
	if (tmp != NULL)
		scheme->priv->name = g_strdup (_((char*) tmp));
	else if ((tmp = xmlGetProp (scheme_node, BAD_CAST "name")) != NULL)
		scheme->priv->name = g_strdup ((char*) tmp);
	else
	{
		g_set_error (error, ERROR_QUARK, 0, "missing 'name' attribute");
		return;
	}
	xmlFree (tmp);

	tmp = xmlGetProp (scheme_node, BAD_CAST "parent-scheme");
	if (tmp != NULL)
		scheme->priv->parent_id = g_strdup ((char*) tmp);
	xmlFree (tmp);

	for (node = scheme_node->children; node != NULL; node = node->next)
		if (node->type == XML_ELEMENT_NODE)
			if (!parse_style_scheme_child (scheme, node, error))
				return;

	/* NULL-terminate the array of authors */
	if (scheme->priv->authors != NULL)
		g_ptr_array_add (scheme->priv->authors, NULL);
}

/*
 * _gtk_source_style_scheme_new_from_file:
 * @filename: file to parse.
 *
 * Returns: (nullable): new #GtkSourceStyleScheme created from @filename, or
 *   %NULL on error.
 */
GtkSourceStyleScheme *
_gtk_source_style_scheme_new_from_file (const gchar *filename)
{
	GtkSourceStyleScheme *scheme;
	gchar *text;
	gsize text_len;
	xmlDoc *doc;
	xmlNode *node;
	GError *error = NULL;

	g_return_val_if_fail (filename != NULL, NULL);

	if (!g_file_get_contents (filename, &text, &text_len, &error))
	{
		gchar *filename_utf8 = g_filename_display_name (filename);
		g_warning ("could not load style scheme file '%s': %s",
			   filename_utf8, error->message);
		g_free (filename_utf8);
		g_clear_error (&error);
		return NULL;
	}

	doc = xmlParseMemory (text, text_len);

	if (!doc)
	{
		gchar *filename_utf8 = g_filename_display_name (filename);
		g_warning ("could not parse scheme file '%s'", filename_utf8);
		g_free (filename_utf8);
		g_free (text);
		return NULL;
	}

	node = xmlDocGetRootElement (doc);

	if (node == NULL)
	{
		gchar *filename_utf8 = g_filename_display_name (filename);
		g_warning ("could not load scheme file '%s': empty document", filename_utf8);
		g_free (filename_utf8);
		xmlFreeDoc (doc);
		g_free (text);
		return NULL;
	}

	scheme = g_object_new (GTK_SOURCE_TYPE_STYLE_SCHEME, NULL);
	scheme->priv->filename = g_strdup (filename);

	parse_style_scheme_element (scheme, node, &error);

	if (error != NULL)
	{
		gchar *filename_utf8 = g_filename_display_name (filename);
		g_warning ("could not load style scheme file '%s': %s",
			   filename_utf8, error->message);
		g_free (filename_utf8);
		g_clear_error (&error);
		g_clear_object (&scheme);
	}

	xmlFreeDoc (doc);
	g_free (text);

	return scheme;
}

/* Returns: (nullable): the parent style scheme ID, or %NULL. */
const gchar *
_gtk_source_style_scheme_get_parent_id (GtkSourceStyleScheme *scheme)
{
	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME (scheme), NULL);

	return scheme->priv->parent_id;
}

void
_gtk_source_style_scheme_set_parent (GtkSourceStyleScheme *scheme,
				     GtkSourceStyleScheme *parent_scheme)
{
	g_return_if_fail (GTK_SOURCE_IS_STYLE_SCHEME (scheme));
	g_return_if_fail (parent_scheme == NULL || GTK_SOURCE_IS_STYLE_SCHEME (parent_scheme));

	g_set_object (&scheme->priv->parent, parent_scheme);
}

GtkSourceStyleSchemeCss *
_gtk_source_style_scheme_get_style_scheme_css (GtkSourceStyleScheme *scheme)
{
	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME (scheme), NULL);

	if (scheme->priv->scheme_css == NULL)
	{
		scheme->priv->scheme_css = _gtk_source_style_scheme_css_new (scheme);
	}

	return scheme->priv->scheme_css;
}
