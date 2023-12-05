/*
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2003 - Paolo Maggi <paolo.maggi@polito.it>
 * Copyright (C) 2023 - Sébastien Wilmet <swilmet@gnome.org>
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

#include "gtksourcestylescheme.h"
#include "gtksourcestylescheme-private.h"
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

struct _GtkSourceStyleSchemePrivate
{
	gchar *filename;

	/* @basic_infos and @styles are what
	 * _gtk_source_style_scheme_parser_parse_file() returns. See that
	 * function and corresponding source file for all the details.
	 */
	GtkSourceStyleSchemeBasicInfos *basic_infos;
	GHashTable *styles;

	GtkSourceStyleSchemeCss *scheme_css;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtkSourceStyleScheme, gtk_source_style_scheme, G_TYPE_OBJECT)

static void
gtk_source_style_scheme_dispose (GObject *object)
{
	GtkSourceStyleScheme *scheme = GTK_SOURCE_STYLE_SCHEME (object);

	if (scheme->priv->styles != NULL)
	{
		g_hash_table_unref (scheme->priv->styles);
		scheme->priv->styles = NULL;
	}

	g_clear_object (&scheme->priv->scheme_css);

	G_OBJECT_CLASS (gtk_source_style_scheme_parent_class)->dispose (object);
}

static void
gtk_source_style_scheme_finalize (GObject *object)
{
	GtkSourceStyleScheme *scheme = GTK_SOURCE_STYLE_SCHEME (object);

	g_free (scheme->priv->filename);
	_gtk_source_style_scheme_basic_infos_free (scheme->priv->basic_infos);

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
gtk_source_style_scheme_init (GtkSourceStyleScheme *scheme)
{
	scheme->priv = gtk_source_style_scheme_get_instance_private (scheme);
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

	if (scheme->priv->basic_infos == NULL)
	{
		return NULL;
	}

	return scheme->priv->basic_infos->id;
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

	if (scheme->priv->basic_infos == NULL)
	{
		return NULL;
	}

	return scheme->priv->basic_infos->name;
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

	if (scheme->priv->basic_infos == NULL)
	{
		return NULL;
	}

	return scheme->priv->basic_infos->description;
}

/**
 * gtk_source_style_scheme_get_kind:
 * @scheme: a #GtkSourceStyleScheme.
 *
 * Returns: the kind of @scheme.
 * Since: 299.2
 */
GtkSourceStyleSchemeKind
gtk_source_style_scheme_get_kind (GtkSourceStyleScheme *scheme)
{
	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME (scheme), GTK_SOURCE_STYLE_SCHEME_KIND_LIGHT);

	if (scheme->priv->basic_infos == NULL)
	{
		return GTK_SOURCE_STYLE_SCHEME_KIND_LIGHT;
	}

	return scheme->priv->basic_infos->kind;
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

/**
 * gtk_source_style_scheme_get_style:
 * @scheme: a #GtkSourceStyleScheme.
 * @style_id: ID of the style to retrieve.
 *
 * Returns: (nullable) (transfer none): the #GtkSourceStyle which corresponds to
 *   @style_id in the @scheme, or %NULL if not found.
 * Since: 2.0
 */
GtkSourceStyle *
gtk_source_style_scheme_get_style (GtkSourceStyleScheme *scheme,
				   const gchar          *style_id)
{
	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME (scheme), NULL);
	g_return_val_if_fail (style_id != NULL, NULL);

	if (scheme->priv->styles == NULL)
	{
		return NULL;
	}

	return g_hash_table_lookup (scheme->priv->styles, style_id);
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
	GFile *file;
	GError *error = NULL;
	gboolean ok;

	g_return_val_if_fail (filename != NULL, NULL);

	scheme = g_object_new (GTK_SOURCE_TYPE_STYLE_SCHEME, NULL);
	scheme->priv->filename = g_strdup (filename);

	file = g_file_new_for_path (filename);
	ok = _gtk_source_style_scheme_parser_parse_file (file,
							 &scheme->priv->basic_infos,
							 &scheme->priv->styles,
							 &error);
	g_object_unref (file);


	if (error != NULL)
	{
		g_warning ("Failed to load style scheme file '%s': %s",
			   filename,
			   error->message);
		g_clear_error (&error);
	}

	if (!ok)
	{
		g_object_unref (scheme);
		return NULL;
	}

	return scheme;
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
