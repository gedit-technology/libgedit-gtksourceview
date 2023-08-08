/*
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2015 - Paolo Borelli
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

static void
set_single_search_path (GtkSourceStyleSchemeManager *manager,
			const gchar                 *path)
{
	const gchar *search_path[] = { path, NULL };

	gtk_source_style_scheme_manager_set_search_path (manager, (gchar **) search_path);
}

/* Control the search path, to have reproducible results.
 * Also, the unit tests can be run without the need to install the project.
 */
static GtkSourceStyleSchemeManager *
create_manager (void)
{
	GtkSourceStyleSchemeManager *manager;
	gchar *dir;

	manager = gtk_source_style_scheme_manager_new ();

	dir = g_build_filename (UNIT_TESTS_SRCDIR, "datasets", "style-schemes", "basics", NULL);
	set_single_search_path (manager, dir);
	g_free (dir);

	return manager;
}

static void
check_scheme (GtkSourceStyleScheme *scheme,
	      const gchar          *expected_id,
	      const gchar          *expected_name,
	      const gchar          *expected_description,
	      const gchar          *style_id)
{
	GtkSourceStyle *style;

	g_assert_cmpstr (gtk_source_style_scheme_get_id (scheme), ==, expected_id);
	g_assert_cmpstr (gtk_source_style_scheme_get_name (scheme), ==, expected_name);
	g_assert_cmpstr (gtk_source_style_scheme_get_description (scheme), ==, expected_description);

	style = gtk_source_style_scheme_get_style (scheme, style_id);
	g_assert_nonnull (style);
}

static void
test_scheme_attributes (void)
{
	GtkSourceStyleSchemeManager *manager = create_manager ();
	GtkSourceStyleScheme *scheme;

	scheme = gtk_source_style_scheme_manager_get_scheme (manager, "test");
	check_scheme (scheme, "test", "Test", "Test color scheme", "def:comment");

	g_object_unref (manager);
}

static void
test_rgba_colors (void)
{
	GtkSourceStyleSchemeManager *manager = create_manager ();
	GtkSourceStyleScheme *scheme = gtk_source_style_scheme_manager_get_scheme (manager, "test");
	GtkSourceStyle *style_current_line;
	GtkSourceStyle *style_background_pattern;

	style_current_line = gtk_source_style_scheme_get_style (scheme, "current-line");
	style_background_pattern = gtk_source_style_scheme_get_style (scheme, "background-pattern");
	g_assert_nonnull (style_current_line);
	g_assert_nonnull (style_background_pattern);

	g_assert_true (style_current_line->use_background_color);
	g_assert_true (style_background_pattern->use_background_color);
	g_assert_true (gdk_rgba_equal (&style_current_line->background_color,
				       &style_background_pattern->background_color));

	g_object_unref (manager);
}

int
main (int    argc,
      char **argv)
{
	gint ret;

	gtk_test_init (&argc, &argv);
	gtk_source_init ();

	g_test_add_func ("/StyleScheme/scheme_attributes", test_scheme_attributes);
	g_test_add_func ("/StyleScheme/rgba_colors", test_rgba_colors);

	ret = g_test_run ();
	gtk_source_finalize ();

	return ret;
}
