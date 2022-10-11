/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*- */
/*
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2005 - Paolo Borelli
 * Copyright (C) 2007 - Gustavo Giráldez
 * Copyright (C) 2007 - Paolo Maggi
 * Copyright (C) 2013-2022 - Sébastien Wilmet <swilmet@gnome.org>
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

/**
 * SECTION:utils
 * @title: GtkSourceUtils
 * @short_description: Utility functions
 *
 * Utility functions.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtksourceutils.h"
#include "gtksourceutils-private.h"
#include <string.h>
#include <errno.h>
#include <math.h>
#include <glib/gi18n-lib.h>

/**
 * gtk_source_utils_unescape_search_text:
 * @text: the text to unescape.
 *
 * Use this function before gtk_source_search_settings_set_search_text(), to
 * unescape the following sequences of characters: `\n`, `\r`, `\t` and `\\`.
 * The purpose is to easily write those characters in a search entry.
 *
 * Note that unescaping the search text is not needed for regular expression
 * searches.
 *
 * See also: gtk_source_utils_escape_search_text().
 *
 * Returns: the unescaped @text.
 * Since: 3.10
 */
gchar *
gtk_source_utils_unescape_search_text (const gchar *text)
{
	GString *str;
	gint length;
	gboolean drop_prev = FALSE;
	const gchar *cur;
	const gchar *end;
	const gchar *prev;

	if (text == NULL)
	{
		return NULL;
	}

	length = strlen (text);

	str = g_string_new ("");

	cur = text;
	end = text + length;
	prev = NULL;

	while (cur != end)
	{
		const gchar *next;
		next = g_utf8_next_char (cur);

		if (prev && (*prev == '\\'))
		{
			switch (*cur)
			{
				case 'n':
					str = g_string_append (str, "\n");
					break;
				case 'r':
					str = g_string_append (str, "\r");
					break;
				case 't':
					str = g_string_append (str, "\t");
					break;
				case '\\':
					str = g_string_append (str, "\\");
					drop_prev = TRUE;
					break;
				default:
					str = g_string_append (str, "\\");
					str = g_string_append_len (str, cur, next - cur);
					break;
			}
		}
		else if (*cur != '\\')
		{
			str = g_string_append_len (str, cur, next - cur);
		}
		else if ((next == end) && (*cur == '\\'))
		{
			str = g_string_append (str, "\\");
		}

		if (!drop_prev)
		{
			prev = cur;
		}
		else
		{
			prev = NULL;
			drop_prev = FALSE;
		}

		cur = next;
	}

	return g_string_free (str, FALSE);
}

/**
 * gtk_source_utils_escape_search_text:
 * @text: the text to escape.
 *
 * Use this function to escape the following characters: `\n`, `\r`, `\t` and `\`.
 *
 * For a regular expression search, use g_regex_escape_string() instead.
 *
 * One possible use case is to take the #GtkTextBuffer's selection and put it in a
 * search entry. The selection can contain tabulations, newlines, etc. So it's
 * better to escape those special characters to better fit in the search entry.
 *
 * See also: gtk_source_utils_unescape_search_text().
 *
 * <warning>
 * Warning: the escape and unescape functions are not reciprocal! For example,
 * escape (unescape (\)) = \\. So avoid cycles such as: search entry -> unescape
 * -> search settings -> escape -> search entry. The original search entry text
 * may be modified.
 * </warning>
 *
 * Returns: the escaped @text.
 * Since: 3.10
 */
gchar *
gtk_source_utils_escape_search_text (const gchar* text)
{
	GString *str;
	gint length;
	const gchar *p;
	const gchar *end;

	if (text == NULL)
	{
		return NULL;
	}

	length = strlen (text);

	str = g_string_new ("");

	p = text;
	end = text + length;

	while (p != end)
	{
		const gchar *next = g_utf8_next_char (p);

		switch (*p)
		{
			case '\n':
				g_string_append (str, "\\n");
				break;
			case '\r':
				g_string_append (str, "\\r");
				break;
			case '\t':
				g_string_append (str, "\\t");
				break;
			case '\\':
				g_string_append (str, "\\\\");
				break;
			default:
				g_string_append_len (str, p, next - p);
				break;
		}

		p = next;
	}

	return g_string_free (str, FALSE);
}

#define GSV_DATA_SUBDIR "libgedit-gtksourceview-" GSV_API_VERSION

gchar **
_gtk_source_utils_get_default_dirs (const gchar *basename)
{
	const gchar * const *system_dirs;
	GPtrArray *dirs;

	dirs = g_ptr_array_new ();

	/* User dir */
	g_ptr_array_add (dirs, g_build_filename (g_get_user_data_dir (),
						 GSV_DATA_SUBDIR,
						 basename,
						 NULL));

	/* System dirs */
	for (system_dirs = g_get_system_data_dirs ();
	     system_dirs != NULL && *system_dirs != NULL;
	     system_dirs++)
	{
		g_ptr_array_add (dirs, g_build_filename (*system_dirs,
							 GSV_DATA_SUBDIR,
							 basename,
							 NULL));
	}

	g_ptr_array_add (dirs, NULL);

	return (gchar **) g_ptr_array_free (dirs, FALSE);
}

static GSList *
build_file_listing (const gchar *item,
		    GSList      *filenames,
		    const gchar *suffix,
		    gboolean     only_dirs)
{
	GDir *dir;
	const gchar *name;

	if (!only_dirs && g_file_test (item, G_FILE_TEST_IS_REGULAR))
	{
		filenames = g_slist_prepend (filenames, g_strdup(item));
		return filenames;

	}
	dir = g_dir_open (item, 0, NULL);

	if (dir == NULL)
	{
		return filenames;
	}

	while ((name = g_dir_read_name (dir)) != NULL)
	{
		gchar *full_path = g_build_filename (item, name, NULL);

		if (!g_file_test (full_path, G_FILE_TEST_IS_DIR) &&
		    g_str_has_suffix (name, suffix))
		{
			filenames = g_slist_prepend (filenames, full_path);
		}
		else
		{
			g_free (full_path);
		}
	}

	g_dir_close (dir);

	return filenames;
}

GSList *
_gtk_source_utils_get_file_list (gchar       **path,
				 const gchar  *suffix,
				 gboolean      only_dirs)
{
	GSList *files = NULL;

	for ( ; path && *path; ++path)
	{
		files = build_file_listing (*path, files, suffix, only_dirs);
	}

	return g_slist_reverse (files);
}

/* Wrapper around strtoull for easier use: tries
 * to convert @str to a number and return -1 if it is not.
 * Used to check if references in subpattern contexts
 * (e.g. \%{1@start} or \%{blah@start}) are named or numbers.
 */
gint
_gtk_source_utils_string_to_int (const gchar *str)
{
	guint64 number;
	gchar *end_str;

	if (str == NULL || *str == '\0')
	{
		return -1;
	}

	errno = 0;
	number = g_ascii_strtoull (str, &end_str, 10);

	if (errno != 0 || number > G_MAXINT || *end_str != '\0')
	{
		return -1;
	}

	return number;
}

/*
 * _gtk_source_utils_find_escaped_char:
 * @str: a valid UTF-8 string. Must not be %NULL.
 * @ch: a character.
 *
 * Finds an escaped character. For @ch to be escaped, it must be preceded by an
 * odd number of backslashes. Usually only one backslash and then @ch.
 *
 * Examples with @ch set to `'C'`:
 * - `"\C"`: C is escaped.
 * - `"\\C"`: C is not escaped, it is preceded by an escaped backslash.
 * - `"\\\C"`: C is escaped.
 *
 * Use-case: know if `"\C"` is present in a #GRegex pattern.
 *
 * Returns: (nullable): the position in @str where @ch has been found to be
 *   escaped (the return value points to a @ch value, not the backslash). Or
 *   %NULL if an escaped @ch is not present in @str.
 */
const gchar *
_gtk_source_utils_find_escaped_char (const gchar *str,
				     gchar        ch)
{
	gboolean escaped = FALSE;
	const gchar *str_pos;

	g_return_val_if_fail (str != NULL, NULL);
	g_return_val_if_fail (g_utf8_validate (str, -1, NULL), NULL);

	for (str_pos = str; *str_pos != '\0'; str_pos++)
	{
		gchar cur_char = *str_pos;

		if (cur_char == '\\')
		{
			escaped = !escaped;
		}
		else if (cur_char == ch && escaped)
		{
			return str_pos;
		}
		else
		{
			escaped = FALSE;
		}
	}

	return NULL;
}

/*
 * _gtk_source_utils_dgettext:
 *
 * Try to translate string from given domain. It returns
 * duplicated string which must be freed with g_free().
 */
gchar *
_gtk_source_utils_dgettext (const gchar *domain,
			    const gchar *string)
{
	const gchar *translated;
	gchar *tmp;

	g_return_val_if_fail (string != NULL, NULL);

	if (domain == NULL)
	{
		return g_strdup (_(string));
	}

	translated = dgettext (domain, string);

	if (g_strcmp0 (translated, string) == 0)
	{
		return g_strdup (_(string));
	}

	if (g_utf8_validate (translated, -1, NULL))
	{
		return g_strdup (translated);
	}

	tmp = g_locale_to_utf8 (translated, -1, NULL, NULL, NULL);
	return tmp != NULL ? tmp : g_strdup (string);
}
