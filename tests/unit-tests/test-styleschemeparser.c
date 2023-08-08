/*
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2023 - Sébastien Wilmet
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
#include "gtksourceview/gtksourcestyleschemeparser.h"

static void
test_parse_final_color (void)
{
	GdkRGBA rgba;

	g_assert_false (_gtk_source_style_scheme_parser_parse_final_color ("", &rgba));

	g_assert_true (_gtk_source_style_scheme_parser_parse_final_color ("#000000", &rgba));
	g_assert_false (_gtk_source_style_scheme_parser_parse_final_color ("000000", &rgba));
	g_assert_false (_gtk_source_style_scheme_parser_parse_final_color ("##000000", &rgba));

	g_assert_true (_gtk_source_style_scheme_parser_parse_final_color ("#black", &rgba));
	g_assert_false (_gtk_source_style_scheme_parser_parse_final_color ("black", &rgba));

	g_assert_true (_gtk_source_style_scheme_parser_parse_final_color ("#rgb(255,0,0)", &rgba));
	g_assert_false (_gtk_source_style_scheme_parser_parse_final_color ("rgb(255,0,0)", &rgba));

	g_assert_true (_gtk_source_style_scheme_parser_parse_final_color ("#rgba(255,0,0,0.5)", &rgba));
	g_assert_false (_gtk_source_style_scheme_parser_parse_final_color ("rgba(255,0,0,0.5)", &rgba));
}

static void
test_parse_scale (void)
{
	gdouble val = 0.0;

	g_assert_false (_gtk_source_style_scheme_parser_parse_scale ("", NULL));
	g_assert_false (_gtk_source_style_scheme_parser_parse_scale ("foo", NULL));
	g_assert_false (_gtk_source_style_scheme_parser_parse_scale ("0", NULL));
	g_assert_false (_gtk_source_style_scheme_parser_parse_scale ("0.0", NULL));
	g_assert_false (_gtk_source_style_scheme_parser_parse_scale ("-1", NULL));

	g_assert_true (_gtk_source_style_scheme_parser_parse_scale ("1.2", NULL));
	g_assert_true (_gtk_source_style_scheme_parser_parse_scale ("1.2", &val));
	g_assert_cmpfloat (val, ==, 1.2);

	g_assert_true (_gtk_source_style_scheme_parser_parse_scale ("x-large", &val));
	g_assert_false (_gtk_source_style_scheme_parser_parse_scale ("X-Large", &val));
	g_assert_false (_gtk_source_style_scheme_parser_parse_scale (" x-large", &val));
}

static void
check_parsing_error_full (const gchar *filename,
			  GQuark       expected_error_domain)
{
	gchar *path;
	GFile *file;
	GError *error = NULL;
	gboolean ok;

	path = g_build_filename (UNIT_TESTS_SRCDIR, "datasets", "style-schemes", "parsing-errors", filename, NULL);
	file = g_file_new_for_path (path);

	ok = _gtk_source_style_scheme_parser_parse_file (file, NULL, NULL, &error);
	g_assert_false (ok);
	g_assert_nonnull (error);
	g_assert_true (error->domain == expected_error_domain);

	g_free (path);
	g_object_unref (file);
	g_error_free (error);
}

static void
check_parsing_error (const gchar *filename)
{
	check_parsing_error_full (filename, G_MARKUP_ERROR);
}

static void
check_successful_parsing (void)
{
	gchar *path;
	GFile *file;
	GError *error = NULL;
	gboolean ok;
	GtkSourceStyleSchemeBasicInfos *basic_infos = NULL;

	path = g_build_filename (UNIT_TESTS_SRCDIR, "datasets", "style-schemes", "basics", "classic.xml", NULL);
	file = g_file_new_for_path (path);

	ok = _gtk_source_style_scheme_parser_parse_file (file, &basic_infos, NULL, &error);
	g_assert_true (ok);
	g_assert_null (error);
	g_assert_cmpstr (basic_infos->id, ==, "classic");
	g_assert_cmpstr (basic_infos->name, ==, "Classic");

	g_free (path);
	g_object_unref (file);
	_gtk_source_style_scheme_basic_infos_free (basic_infos);
}

static void
test_parse_file (void)
{
	check_parsing_error_full ("file-not-found", G_IO_ERROR);
	check_parsing_error ("001-no-style-scheme-attributes.xml");
	check_parsing_error ("002-nested-style-scheme-elements.xml");
	check_parsing_error ("003-two-style-scheme-elements.xml");
	check_parsing_error ("004-empty-id.xml");
	check_parsing_error ("005-no-name.xml");
	check_parsing_error ("006-empty-names.xml");
	check_parsing_error ("007-two-names.xml");
	check_parsing_error ("008-two-descriptions.xml");

	check_successful_parsing ();
}

int
main (int    argc,
      char **argv)
{
	gint ret;

	gtk_test_init (&argc, &argv);
	gtk_source_init ();

	g_test_add_func ("/StyleSchemeParser/parse_final_color", test_parse_final_color);
	g_test_add_func ("/StyleSchemeParser/parse_scale", test_parse_scale);
	g_test_add_func ("/StyleSchemeParser/parse_file", test_parse_file);

	ret = g_test_run ();
	gtk_source_finalize ();

	return ret;
}
