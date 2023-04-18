/*
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2013 - Sébastien Wilmet <swilmet@gnome.org>
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

#include <gtksourceview/gtksource.h>
#include "gtksourceview/gtksourceutils-private.h"

static void
test_unescape_search_text (void)
{
	gchar *unescaped_text;

	unescaped_text = gtk_source_utils_unescape_search_text ("\\n");
	g_assert_cmpstr (unescaped_text, ==, "\n");
	g_free (unescaped_text);

	unescaped_text = gtk_source_utils_unescape_search_text ("\\r");
	g_assert_cmpstr (unescaped_text, ==, "\r");
	g_free (unescaped_text);

	unescaped_text = gtk_source_utils_unescape_search_text ("\\t");
	g_assert_cmpstr (unescaped_text, ==, "\t");
	g_free (unescaped_text);

	unescaped_text = gtk_source_utils_unescape_search_text ("\\\\");
	g_assert_cmpstr (unescaped_text, ==, "\\");
	g_free (unescaped_text);

	unescaped_text = gtk_source_utils_unescape_search_text ("foo\\n bar\\r ß\\t hello\\\\blah");
	g_assert_cmpstr (unescaped_text, ==, "foo\n bar\r ß\t hello\\blah");
	g_free (unescaped_text);

	unescaped_text = gtk_source_utils_unescape_search_text ("foo\n bar\r ß\t hello\\blah");
	g_assert_cmpstr (unescaped_text, ==, "foo\n bar\r ß\t hello\\blah");
	g_free (unescaped_text);
}

static void
test_escape_search_text (void)
{
	gchar *escaped_text;

	escaped_text = gtk_source_utils_escape_search_text ("\n");
	g_assert_cmpstr (escaped_text, ==, "\\n");
	g_free (escaped_text);

	escaped_text = gtk_source_utils_escape_search_text ("\r");
	g_assert_cmpstr (escaped_text, ==, "\\r");
	g_free (escaped_text);

	escaped_text = gtk_source_utils_escape_search_text ("\t");
	g_assert_cmpstr (escaped_text, ==, "\\t");
	g_free (escaped_text);

	escaped_text = gtk_source_utils_escape_search_text ("\\");
	g_assert_cmpstr (escaped_text, ==, "\\\\");
	g_free (escaped_text);

	escaped_text = gtk_source_utils_escape_search_text ("foo\n bar\r ß\t hello\\blah");
	g_assert_cmpstr (escaped_text, ==, "foo\\n bar\\r ß\\t hello\\\\blah");
	g_free (escaped_text);
}

static void
test_find_escaped_char (void)
{
	const gchar *str_pos;
	const gchar *str;

	/* Without backslashes */
	str_pos = _gtk_source_utils_find_escaped_char ("", 'C');
	g_assert_null (str_pos);

	str_pos = _gtk_source_utils_find_escaped_char ("a", 'C');
	g_assert_null (str_pos);

	str_pos = _gtk_source_utils_find_escaped_char ("abc", 'C');
	g_assert_null (str_pos);

	str_pos = _gtk_source_utils_find_escaped_char ("C", 'C');
	g_assert_null (str_pos);

	str_pos = _gtk_source_utils_find_escaped_char ("abC", 'C');
	g_assert_null (str_pos);

	str_pos = _gtk_source_utils_find_escaped_char ("Cde", 'C');
	g_assert_null (str_pos);

	str_pos = _gtk_source_utils_find_escaped_char ("UTF-8 éèç C blah", 'C');
	g_assert_null (str_pos);

	/* With backslash */
	str_pos = _gtk_source_utils_find_escaped_char ("\\D", 'C');
	g_assert_null (str_pos);

	str_pos = _gtk_source_utils_find_escaped_char ("\\DC", 'C');
	g_assert_null (str_pos);

	str_pos = _gtk_source_utils_find_escaped_char ("\\\\C", 'C');
	g_assert_null (str_pos);

	str = "\\C";
	str_pos = _gtk_source_utils_find_escaped_char (str, 'C');
	g_assert_nonnull (str_pos);
	g_assert_cmpint (str_pos - str, ==, 1);

	str = "\\D\\C";
	str_pos = _gtk_source_utils_find_escaped_char (str, 'C');
	g_assert_nonnull (str_pos);
	g_assert_cmpint (str_pos - str, ==, 3);

	str = "CCC\\C";
	str_pos = _gtk_source_utils_find_escaped_char (str, 'C');
	g_assert_nonnull (str_pos);
	g_assert_cmpint (str_pos - str, ==, 4);

	str = "a \\\\\\Cosinus";
	str_pos = _gtk_source_utils_find_escaped_char (str, 'C');
	g_assert_nonnull (str_pos);
	g_assert_cmpint (str_pos - str, ==, 5);
}

int
main (int    argc,
      char **argv)
{
	gint ret;

	gtk_test_init (&argc, &argv);
	gtk_source_init ();

	g_test_add_func ("/Utils/unescape_search_text", test_unescape_search_text);
	g_test_add_func ("/Utils/escape_search_text", test_escape_search_text);
	g_test_add_func ("/Utils/find_escaped_char", test_find_escaped_char);

	ret = g_test_run ();
	gtk_source_finalize ();

	return ret;
}
