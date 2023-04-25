/*
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2013 - Paolo Borelli
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

static void
test_get_default (void)
{
	GtkSourceStyleSchemeManager *default_manager1;
	GtkSourceStyleSchemeManager *default_manager2;

	default_manager1 = gtk_source_style_scheme_manager_get_default ();
	default_manager2 = gtk_source_style_scheme_manager_get_default ();
	g_assert (default_manager1 == default_manager2);
}

static void
test_prepend_search_path (void)
{
	GtkSourceStyleSchemeManager *manager;
	gchar *styles_test_dir;
	GtkSourceStyleScheme *scheme;
	const gchar *obtained_filename;
	gchar *expected_filename;

	manager = gtk_source_style_scheme_manager_get_default ();

	styles_test_dir = g_test_build_filename (G_TEST_DIST, "datasets", "style-schemes", "basics", NULL);
	gtk_source_style_scheme_manager_prepend_search_path (manager, styles_test_dir);

	scheme = gtk_source_style_scheme_manager_get_scheme (manager, "classic");
	obtained_filename = gtk_source_style_scheme_get_filename (scheme);
	expected_filename = g_build_filename (styles_test_dir, "classic.xml", NULL);
	g_assert_cmpstr (obtained_filename, ==, expected_filename);

	g_free (styles_test_dir);
	g_free (expected_filename);
}

int
main (int    argc,
      char **argv)
{
	gint ret;

	gtk_test_init (&argc, &argv);
	gtk_source_init ();

	g_test_add_func ("/StyleSchemeManager/get_default", test_get_default);
	g_test_add_func ("/StyleSchemeManager/prepend_search_path", test_prepend_search_path);

	ret = g_test_run ();
	gtk_source_finalize ();

	return ret;
}
