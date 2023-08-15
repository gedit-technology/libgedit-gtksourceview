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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtksourcestyleschemeparser.h"
#include <glib/gi18n-lib.h>
#include "gtksourcestyle.h"

typedef struct
{
	GMarkupParser markup_parser;
	GMarkupParseContext *markup_parse_context;

	GtkSourceStyleSchemeBasicInfos *basic_infos;

	/* GMarkupParser.text() can be called several times for the same
	 * element [1], so we concatenate the chunks.
	 *
	 * [1] For example in HTML with: <p>foo<br />bar</p>
	 * Two chunks: "foo" and "bar".
	 */
	GString *text_content;

	/* For storing the data of <color> tags.
	 * Keys: type "owned gchar*": color name.
	 * Values: type "owned GdkRGBA*".
	 */
	GHashTable *named_colors;

	/* For storing the data of <style> tags.
	 * Keys: type "owned gchar*": style name.
	 * Values: type "owned GtkSourceStyle*".
	 */
	GHashTable *styles;
} ParserData;

static GtkSourceStyleSchemeBasicInfos *
basic_infos_new (void)
{
	return g_new0 (GtkSourceStyleSchemeBasicInfos, 1);
}

void
_gtk_source_style_scheme_basic_infos_free (GtkSourceStyleSchemeBasicInfos *basic_infos)
{
	if (basic_infos != NULL)
	{
		g_free (basic_infos->id);
		g_free (basic_infos->name);
		g_free (basic_infos->description);

		g_free (basic_infos);
	}
}

/*
 * _gtk_source_style_scheme_parser_parse_final_color:
 * @color_str: the string to parse.
 * @rgba: (out):
 *
 * This function parses a string that should contain a final color value.
 *
 * It can be either:
 * - For the `<color>` tag: the content of the `value` attribute.
 * - For the `<style>` tag: the content of an attribute taking a color as value,
 *   but that doesn't reference a `<color>` tag.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 */
gboolean
_gtk_source_style_scheme_parser_parse_final_color (const gchar *color_str,
						   GdkRGBA     *rgba)
{
	g_return_val_if_fail (color_str != NULL, FALSE);
	g_return_val_if_fail (rgba != NULL, FALSE);

	/* All final color values must start with #, so when reading a style
	 * scheme file we directly know that it's a final value.
	 * References to a <color> tag never start with #.
	 */
	if (color_str[0] != '#')
	{
		return FALSE;
	}

	/* To avoid for example "##000000" being accepted (see below). */
	if (color_str[1] == '#')
	{
		return FALSE;
	}

	/* Something like "#000000" */
	if (gdk_rgba_parse (rgba, color_str))
	{
		return TRUE;
	}

	/* Skip the # prefix, to accept the other variants. */
	return gdk_rgba_parse (rgba, color_str + 1);
}

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

static gboolean
is_empty (const gchar *str)
{
	return str == NULL || *str == '\0';
}

static gboolean
has_valid_element_stack (GMarkupParseContext *markup_parse_context)
{
	const GSList *element_stack;

	element_stack = g_markup_parse_context_get_element_stack (markup_parse_context);

	if (element_stack == NULL || element_stack->data == NULL)
	{
		return FALSE;
	}

	/* <style-scheme> is the top-level element. */
	if (g_str_equal (element_stack->data, "style-scheme"))
	{
		return element_stack->next == NULL;
	}

	/* The immediate children of <style-scheme>. */
	if (g_str_equal (element_stack->data, "description") ||
	    g_str_equal (element_stack->data, "_description") ||
	    g_str_equal (element_stack->data, "color") ||
	    g_str_equal (element_stack->data, "style"))
	{
		const GSList *parent = element_stack->next;

		return (parent != NULL &&
			g_strcmp0 (parent->data, "style-scheme") == 0 &&
			parent->next == NULL);
	}

	return FALSE;
}

static gboolean
check_element_stack (GMarkupParseContext  *markup_parse_context,
		     GError              **error)
{
	if (!has_valid_element_stack (markup_parse_context))
	{
		g_set_error_literal (error,
				     G_MARKUP_ERROR,
				     G_MARKUP_ERROR_PARSE,
				     "Bad element stack.");
		return FALSE;
	}

	return TRUE;
}

static void
parse_start_element_style_scheme (ParserData   *parser_data,
				  const gchar  *element_name,
				  const gchar **attribute_names,
				  const gchar **attribute_values,
				  GError      **error)
{
	const gchar *id = NULL;
	const gchar *name = NULL;
	const gchar *translatable_name = NULL;
	const gchar *kind = NULL;

	if (parser_data->basic_infos->id != NULL)
	{
		g_set_error_literal (error,
				     G_MARKUP_ERROR,
				     G_MARKUP_ERROR_PARSE,
				     "There must be only one <style-scheme> element.");
		return;
	}

	if (!g_markup_collect_attributes (element_name,
					  attribute_names,
					  attribute_values,
					  error,
					  G_MARKUP_COLLECT_STRING, "id", &id,
					  G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "name", &name,
					  G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "_name", &translatable_name,
					  G_MARKUP_COLLECT_STRING, "kind", &kind,
					  G_MARKUP_COLLECT_INVALID))
	{
		return;
	}

	/* ID */

	if (is_empty (id))
	{
		g_set_error_literal (error,
				     G_MARKUP_ERROR,
				     G_MARKUP_ERROR_INVALID_CONTENT,
				     "<style-scheme> requires a non-empty 'id' attribute value.");
		return;
	}

	parser_data->basic_infos->id = g_strdup (id);

	/* Name */

	if (is_empty (name) && is_empty (translatable_name))
	{
		g_set_error_literal (error,
				     G_MARKUP_ERROR,
				     G_MARKUP_ERROR_MISSING_ATTRIBUTE,
				     "<style-scheme> requires a non-empty 'name' or '_name' attribute value.");
		return;
	}

	if (!is_empty (name) && !is_empty (translatable_name))
	{
		g_set_error_literal (error,
				     G_MARKUP_ERROR,
				     G_MARKUP_ERROR_PARSE,
				     "<style-scheme> cannot have both the 'name' and '_name' attributes set.");
		return;
	}

	if (is_empty (name))
	{
		parser_data->basic_infos->name = g_strdup (_(translatable_name));
	}
	else
	{
		parser_data->basic_infos->name = g_strdup (name);
	}

	/* Kind */

	if (g_strcmp0 (kind, "light") == 0)
	{
		parser_data->basic_infos->kind = GTK_SOURCE_STYLE_SCHEME_KIND_LIGHT;
	}
	else if (g_strcmp0 (kind, "dark") == 0)
	{
		parser_data->basic_infos->kind = GTK_SOURCE_STYLE_SCHEME_KIND_DARK;
	}
	else
	{
		g_set_error (error,
			     G_MARKUP_ERROR,
			     G_MARKUP_ERROR_PARSE,
			     "Failed to parse the kind '%s'.",
			     kind);
		return;
	}
}

static void
parse_start_element_color (ParserData   *parser_data,
			   const gchar  *element_name,
			   const gchar **attribute_names,
			   const gchar **attribute_values,
			   GError      **error)
{
	const gchar *name = NULL;
	const gchar *value = NULL;
	GdkRGBA rgba;

	if (!g_markup_collect_attributes (element_name,
					  attribute_names,
					  attribute_values,
					  error,
					  G_MARKUP_COLLECT_STRING, "name", &name,
					  G_MARKUP_COLLECT_STRING, "value", &value,
					  G_MARKUP_COLLECT_INVALID))
	{
		return;
	}

	if (is_empty (name))
	{
		g_set_error_literal (error,
				     G_MARKUP_ERROR,
				     G_MARKUP_ERROR_INVALID_CONTENT,
				     "<color> requires a non-empty 'name' attribute value.");
		return;
	}

	if (g_hash_table_contains (parser_data->named_colors, name))
	{
		g_set_error (error,
			     G_MARKUP_ERROR,
			     G_MARKUP_ERROR_INVALID_CONTENT,
			     "A <color> tag with the name “%s” already exists.",
			     name);
		return;
	}

	if (!_gtk_source_style_scheme_parser_parse_final_color (value, &rgba))
	{
		g_set_error (error,
			     G_MARKUP_ERROR,
			     G_MARKUP_ERROR_INVALID_CONTENT,
			     "Failed to parse the value “%s” for the <color> tag with name “%s”.",
			     value,
			     name);
		return;
	}

	g_hash_table_insert (parser_data->named_colors,
			     g_strdup (name),
			     gdk_rgba_copy (&rgba));
}

static gboolean
parse_style_color (ParserData   *parser_data,
		   const gchar  *color_str,
		   gboolean     *use,
		   GdkRGBA      *rgba,
		   GError      **error)
{
	GdkRGBA *rgba_from_color_tag;

	if (color_str == NULL)
	{
		*use = FALSE;
		return TRUE;
	}

	rgba_from_color_tag = g_hash_table_lookup (parser_data->named_colors, color_str);
	if (rgba_from_color_tag != NULL)
	{
		*use = TRUE;
		*rgba = *rgba_from_color_tag;
		return TRUE;
	}

	if (_gtk_source_style_scheme_parser_parse_final_color (color_str, rgba))
	{
		*use = TRUE;
		return TRUE;
	}

	*use = FALSE;
	g_set_error (error,
		     G_MARKUP_ERROR,
		     G_MARKUP_ERROR_INVALID_CONTENT,
		     "Failed to parse the color value “%s” for the <style> tag.",
		     color_str);
	return FALSE;
}

static gboolean
parse_style_boolean (const gchar  *value_str,
		     gboolean     *use,
		     gboolean     *value,
		     GError      **error)
{
	if (value_str == NULL)
	{
		*use = FALSE;
		return TRUE;
	}
	if (g_str_equal (value_str, "true"))
	{
		*use = TRUE;
		*value = TRUE;
		return TRUE;
	}
	if (g_str_equal (value_str, "false"))
	{
		*use = TRUE;
		*value = FALSE;
		return TRUE;
	}

	*use = FALSE;
	g_set_error (error,
		     G_MARKUP_ERROR,
		     G_MARKUP_ERROR_INVALID_CONTENT,
		     "Failed to parse the boolean value “%s” for the <style> tag. "
		     "It must be either “true” or “false”.",
		     value_str);
	return FALSE;
}

static gboolean
parse_style_foreground (ParserData      *parser_data,
			const gchar     *foreground,
			GtkSourceStyle  *style,
			GError         **error)
{
	gboolean use = FALSE;

	if (parse_style_color (parser_data,
			       foreground,
			       &use,
			       &style->foreground_color,
			       error))
	{
		style->use_foreground_color = use != FALSE;
		return TRUE;
	}

	return FALSE;
}

static gboolean
parse_style_background (ParserData      *parser_data,
			const gchar     *background,
			GtkSourceStyle  *style,
			GError         **error)
{
	gboolean use = FALSE;

	if (parse_style_color (parser_data,
			       background,
			       &use,
			       &style->background_color,
			       error))
	{
		style->use_background_color = use != FALSE;
		return TRUE;
	}

	return FALSE;
}

static gboolean
parse_style_italic (const gchar     *italic,
		    GtkSourceStyle  *style,
		    GError         **error)
{
	gboolean use = FALSE;
	gboolean value = FALSE;

	if (parse_style_boolean (italic, &use, &value, error))
	{
		style->use_italic = use != FALSE;
		style->italic = value != FALSE;
		return TRUE;
	}

	return FALSE;
}

static gboolean
parse_style_bold (const gchar     *bold,
		  GtkSourceStyle  *style,
		  GError         **error)
{
	gboolean use = FALSE;
	gboolean value = FALSE;

	if (parse_style_boolean (bold, &use, &value, error))
	{
		style->use_bold = use != FALSE;
		style->bold = value != FALSE;
		return TRUE;
	}

	return FALSE;
}

static gboolean
parse_style_underline (const gchar     *underline,
		       GtkSourceStyle  *style,
		       GError         **error)
{
	gboolean use = FALSE;
	gboolean boolean_value = FALSE;
	GEnumClass *enum_class;
	GEnumValue *enum_value;
	gboolean enum_value_found = FALSE;

	if (parse_style_boolean (underline, &use, &boolean_value, NULL))
	{
		style->use_underline = use != FALSE;
		style->underline = boolean_value ? PANGO_UNDERLINE_SINGLE : PANGO_UNDERLINE_NONE;
		return TRUE;
	}

	enum_class = G_ENUM_CLASS (g_type_class_ref (PANGO_TYPE_UNDERLINE));
	enum_value = g_enum_get_value_by_nick (enum_class, underline);
	if (enum_value != NULL)
	{
		style->use_underline = TRUE;
		style->underline = enum_value->value;
		enum_value_found = TRUE;
	}
	g_type_class_unref (enum_class);

	if (!enum_value_found)
	{
		g_set_error (error,
			     G_MARKUP_ERROR,
			     G_MARKUP_ERROR_INVALID_CONTENT,
			     "Failed to parse the underline value “%s” for the <style> tag.",
			     underline);
	}

	return enum_value_found;
}

static gboolean
parse_style_underline_color (ParserData      *parser_data,
			     const gchar     *underline_color,
			     GtkSourceStyle  *style,
			     GError         **error)
{
	gboolean use = FALSE;

	if (parse_style_color (parser_data,
			       underline_color,
			       &use,
			       &style->underline_color,
			       error))
	{
		style->use_underline_color = use != FALSE;
		return TRUE;
	}

	return FALSE;
}

static gboolean
parse_style_strikethrough (const gchar     *strikethrough,
			   GtkSourceStyle  *style,
			   GError         **error)
{
	gboolean use = FALSE;
	gboolean value = FALSE;

	if (parse_style_boolean (strikethrough, &use, &value, error))
	{
		style->use_strikethrough = use != FALSE;
		style->strikethrough = value != FALSE;
		return TRUE;
	}

	return FALSE;
}

static gboolean
parse_style_scale (const gchar     *scale,
		   GtkSourceStyle  *style,
		   GError         **error)
{
	if (scale == NULL)
	{
		style->use_scale = FALSE;
		return TRUE;
	}

	if (_gtk_source_style_scheme_parser_parse_scale (scale, &style->scale))
	{
		style->use_scale = TRUE;
		return TRUE;
	}

	style->use_scale = FALSE;
	g_set_error (error,
		     G_MARKUP_ERROR,
		     G_MARKUP_ERROR_INVALID_CONTENT,
		     "Failed to parse the scale value “%s” for the <style> tag.",
		     scale);
	return FALSE;
}

static void
parse_start_element_style (ParserData   *parser_data,
			   const gchar  *element_name,
			   const gchar **attribute_names,
			   const gchar **attribute_values,
			   GError      **error)
{
	const gchar *name = NULL;
	const gchar *foreground = NULL;
	const gchar *background = NULL;
	const gchar *italic = NULL;
	const gchar *bold = NULL;
	const gchar *underline = NULL;
	const gchar *underline_color = NULL;
	const gchar *strikethrough = NULL;
	const gchar *scale = NULL;
	const gchar *use_style = NULL;
	GtkSourceStyle *style;

	if (!g_markup_collect_attributes (element_name,
					  attribute_names,
					  attribute_values,
					  error,
					  G_MARKUP_COLLECT_STRING, "name", &name,
					  G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "foreground", &foreground,
					  G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "background", &background,
					  G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "italic", &italic,
					  G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "bold", &bold,
					  G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "underline", &underline,
					  G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "underline_color", &underline_color,
					  G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "strikethrough", &strikethrough,
					  G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "scale", &scale,
					  G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "use-style", &use_style,
					  G_MARKUP_COLLECT_INVALID))
	{
		return;
	}

	if (is_empty (name))
	{
		g_set_error_literal (error,
				     G_MARKUP_ERROR,
				     G_MARKUP_ERROR_INVALID_CONTENT,
				     "<style> requires a non-empty 'name' attribute value.");
		return;
	}

	if (g_hash_table_contains (parser_data->styles, name))
	{
		g_set_error (error,
			     G_MARKUP_ERROR,
			     G_MARKUP_ERROR_INVALID_CONTENT,
			     "A <style> tag with the name “%s” already exists.",
			     name);
		return;
	}

	if (foreground == NULL &&
	    background == NULL &&
	    italic == NULL &&
	    bold == NULL &&
	    underline == NULL &&
	    underline_color == NULL &&
	    strikethrough == NULL &&
	    scale == NULL &&
	    use_style == NULL)
	{
		g_set_error (error,
			     G_MARKUP_ERROR,
			     G_MARKUP_ERROR_INVALID_CONTENT,
			     "The <style> tag with the name “%s” has no other attributes than the name. "
			     "A <style> tag must contain at least one other attribute.",
			     name);
		return;
	}

	if (use_style != NULL &&
	    (foreground != NULL ||
	     background != NULL ||
	     italic != NULL ||
	     bold != NULL ||
	     underline != NULL ||
	     underline_color != NULL ||
	     strikethrough != NULL ||
	     scale != NULL))
	{
		g_set_error (error,
			     G_MARKUP_ERROR,
			     G_MARKUP_ERROR_INVALID_CONTENT,
			     "The <style> tag with the name “%s” has the use-style attribute set "
			     "along with other style attributes. If the use-style attribute is used, "
			     "the other attributes must not be used at the exception of the name.",
			     name);
		return;
	}

	if (use_style != NULL)
	{
		style = g_hash_table_lookup (parser_data->styles, use_style);

		if (style != NULL)
		{
			/* Success */
			g_hash_table_insert (parser_data->styles,
					     g_strdup (name),
					     gtk_source_style_ref (style));
		}
		else
		{
			g_set_error (error,
				     G_MARKUP_ERROR,
				     G_MARKUP_ERROR_INVALID_CONTENT,
				     "For the <style> tag with the name “%s”, the use-style value “%s” "
				     "does not refer to a previously defined <style>.",
				     name, use_style);
		}

		return;
	}

	style = gtk_source_style_new ();

	if (parse_style_foreground (parser_data, foreground, style, error) &&
	    parse_style_background (parser_data, background, style, error) &&
	    parse_style_italic (italic, style, error) &&
	    parse_style_bold (bold, style, error) &&
	    parse_style_underline (underline, style, error) &&
	    parse_style_underline_color (parser_data, underline_color, style, error) &&
	    parse_style_strikethrough (strikethrough, style, error) &&
	    parse_style_scale (scale, style, error))
	{
		/* Success */
		g_hash_table_insert (parser_data->styles,
				     g_strdup (name),
				     style);
		return;
	}

	/* Failure */
	gtk_source_style_unref (style);
}

static void
markup_parser_start_element_cb (GMarkupParseContext  *context,
				const gchar          *element_name,
				const gchar         **attribute_names,
				const gchar         **attribute_values,
				gpointer              user_data,
				GError              **error)
{
	ParserData *parser_data = user_data;

	if (!check_element_stack (context, error))
	{
		return;
	}

	if (parser_data->text_content != NULL)
	{
		g_string_free (parser_data->text_content, TRUE);
		parser_data->text_content = NULL;
	}

	if (g_strcmp0 (element_name, "style-scheme") == 0)
	{
		parse_start_element_style_scheme (parser_data,
						  element_name,
						  attribute_names,
						  attribute_values,
						  error);
	}
	else if (g_strcmp0 (element_name, "description") == 0 ||
		 g_strcmp0 (element_name, "_description") == 0)
	{
		if (attribute_names != NULL && attribute_names[0] != NULL)
		{
			g_set_error (error,
				     G_MARKUP_ERROR,
				     G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE,
				     "<%s> must contain zero attributes.",
				     element_name);
		}
	}
	else if (g_strcmp0 (element_name, "color") == 0)
	{
		parse_start_element_color (parser_data,
					   element_name,
					   attribute_names,
					   attribute_values,
					   error);
	}
	else if (g_strcmp0 (element_name, "style") == 0)
	{
		parse_start_element_style (parser_data,
					   element_name,
					   attribute_names,
					   attribute_values,
					   error);
	}
}

static gchar *
get_text_content (ParserData   *parser_data,
		  const gchar  *element_name,
		  GError      **error)
{
	gchar *content = NULL;

	if (parser_data->text_content != NULL)
	{
		content = g_string_free (parser_data->text_content, FALSE);
		parser_data->text_content = NULL;
	}

	if (is_empty (content))
	{
		g_set_error (error,
			     G_MARKUP_ERROR,
			     G_MARKUP_ERROR_INVALID_CONTENT,
			     "<%s>, if present, must not be empty.",
			     element_name);
		g_free (content);
		return NULL;
	}

	return content;
}

static void
set_description (ParserData   *parser_data,
		 const gchar  *description,
		 GError      **error)
{
	if (parser_data->basic_infos->description != NULL)
	{
		g_set_error_literal (error,
				     G_MARKUP_ERROR,
				     G_MARKUP_ERROR_INVALID_CONTENT,
				     "A description has already been provided.");
		return;
	}

	parser_data->basic_infos->description = g_strdup (description);
}

static void
markup_parser_end_element_cb (GMarkupParseContext  *context,
			      const gchar          *element_name,
			      gpointer              user_data,
			      GError              **error)
{
	ParserData *parser_data = user_data;

	if (g_strcmp0 (element_name, "description") == 0)
	{
		gchar *description = get_text_content (parser_data, element_name, error);

		if (description != NULL)
		{
			set_description (parser_data, description, error);
			g_free (description);
		}
	}
	else if (g_strcmp0 (element_name, "_description") == 0)
	{
		gchar *description = get_text_content (parser_data, element_name, error);

		if (description != NULL)
		{
			set_description (parser_data, _(description), error);
			g_free (description);
		}
	}
}

static void
markup_parser_text_cb (GMarkupParseContext  *context,
		       const gchar          *text,
		       gsize                 text_len,
		       gpointer              user_data,
		       GError              **error)
{
	ParserData *parser_data = user_data;
	const gchar *element_name;

	element_name = g_markup_parse_context_get_element (context);

	if (g_strcmp0 (element_name, "description") == 0 ||
	    g_strcmp0 (element_name, "_description") == 0)
	{
		if (parser_data->text_content == NULL)
		{
			parser_data->text_content = g_string_new (NULL);
		}

		g_string_append_len (parser_data->text_content, text, text_len);
	}
}

static ParserData *
parser_data_new (void)
{
	ParserData *parser_data;

	parser_data = g_new0 (ParserData, 1);

	parser_data->markup_parser.start_element = markup_parser_start_element_cb;
	parser_data->markup_parser.end_element = markup_parser_end_element_cb;
	parser_data->markup_parser.text = markup_parser_text_cb;

	parser_data->markup_parse_context = g_markup_parse_context_new (&parser_data->markup_parser,
									G_MARKUP_PREFIX_ERROR_POSITION,
									parser_data,
									NULL);

	parser_data->basic_infos = basic_infos_new ();

	parser_data->named_colors = g_hash_table_new_full (g_str_hash,
							   g_str_equal,
							   g_free,
							   (GDestroyNotify) gdk_rgba_free);

	parser_data->styles = g_hash_table_new_full (g_str_hash,
						     g_str_equal,
						     g_free,
						     (GDestroyNotify) gtk_source_style_unref);

	return parser_data;
}

static void
parser_data_free (ParserData *parser_data)
{
	if (parser_data == NULL)
	{
		return;
	}

	if (parser_data->markup_parse_context != NULL)
	{
		g_markup_parse_context_free (parser_data->markup_parse_context);
	}

	_gtk_source_style_scheme_basic_infos_free (parser_data->basic_infos);

	if (parser_data->text_content != NULL)
	{
		g_string_free (parser_data->text_content, TRUE);
	}

	if (parser_data->named_colors != NULL)
	{
		g_hash_table_unref (parser_data->named_colors);
	}

	if (parser_data->styles != NULL)
	{
		g_hash_table_unref (parser_data->styles);
	}

	g_free (parser_data);
}

/*
 * _gtk_source_style_scheme_parser_parse_file:
 * @file:
 * @basic_infos: (out) (optional): free with _gtk_source_style_scheme_basic_infos_free().
 * @styles: (out) (optional): free with g_hash_table_unref().
 * @error:
 *
 * Returns: %TRUE on success, %FALSE if an error occurred.
 */
gboolean
_gtk_source_style_scheme_parser_parse_file (GFile                           *file,
					    GtkSourceStyleSchemeBasicInfos **basic_infos,
					    GHashTable                     **styles,
					    GError                         **error)
{
	GFileInputStream *input_stream;
	ParserData *parser_data;
	gboolean ok = TRUE;

	g_return_val_if_fail (G_IS_FILE (file), FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	input_stream = g_file_read (file, NULL, error);
	if (input_stream == NULL)
	{
		return FALSE;
	}

	parser_data = parser_data_new ();

	while (ok)
	{
		GBytes *bytes;

		bytes = g_input_stream_read_bytes (G_INPUT_STREAM (input_stream),
						   4096,
						   NULL,
						   error);

		if (bytes == NULL)
		{
			ok = FALSE;
			break;
		}

		if (g_bytes_get_size (bytes) == 0)
		{
			ok = g_markup_parse_context_end_parse (parser_data->markup_parse_context,
							       error);

			g_bytes_unref (bytes);
			break;
		}

		ok = g_markup_parse_context_parse (parser_data->markup_parse_context,
						   g_bytes_get_data (bytes, NULL),
						   g_bytes_get_size (bytes),
						   error);

		g_bytes_unref (bytes);
	}

	if (!g_input_stream_close (G_INPUT_STREAM (input_stream),
				   NULL,
				   error))
	{
		ok = FALSE;
	}

	if (ok)
	{
		if (basic_infos != NULL)
		{
			*basic_infos = parser_data->basic_infos;
			parser_data->basic_infos = NULL;
		}
		if (styles != NULL)
		{
			*styles = parser_data->styles;
			parser_data->styles = NULL;
		}
	}

	g_object_unref (input_stream);
	parser_data_free (parser_data);
	return ok;
}
