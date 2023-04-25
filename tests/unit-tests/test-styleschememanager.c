/*
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2013 - Paolo Borelli
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

/* To have reproducible results.
 * The unit tests can be run without the need to install the project.
 */
static GtkSourceStyleSchemeManager *
create_manager (void)
{
	GtkSourceStyleSchemeManager *manager;
	gchar *dir;

	manager = gtk_source_style_scheme_manager_new ();

	dir = g_build_filename (TOP_SRCDIR, "data", "style-schemes", "default-style-schemes", NULL);
	set_single_search_path (manager, dir);
	g_free (dir);

	return manager;
}

static void
test_get_default (void)
{
	GtkSourceStyleSchemeManager *default_manager1;
	GtkSourceStyleSchemeManager *default_manager2;
	GtkSourceStyleSchemeManager *other_manager;

	default_manager1 = gtk_source_style_scheme_manager_get_default ();
	default_manager2 = gtk_source_style_scheme_manager_get_default ();
	g_assert_true (default_manager1 == default_manager2);

	other_manager = gtk_source_style_scheme_manager_new ();
	g_assert_true (default_manager1 != other_manager);
	g_object_unref (other_manager);
}

static void
test_prepend_search_path (void)
{
	GtkSourceStyleSchemeManager *manager;
	GtkSourceStyleScheme *scheme;
	gchar *obtained_filename_before;
	const gchar *obtained_filename_after;
	gchar *dataset_dir;
	gchar *expected_filename;

	manager = create_manager ();

	// Currently, only the default style schemes are loaded.
	scheme = gtk_source_style_scheme_manager_get_scheme (manager, "classic");
	g_assert_true (scheme != NULL);
	obtained_filename_before = g_strdup (gtk_source_style_scheme_get_filename (scheme));

	// Now prepend the basics/ dir to the search path.
	dataset_dir = g_test_build_filename (G_TEST_DIST, "datasets", "style-schemes", "basics", NULL);
	gtk_source_style_scheme_manager_prepend_search_path (manager, dataset_dir);

	scheme = gtk_source_style_scheme_manager_get_scheme (manager, "classic");
	g_assert_true (scheme != NULL);

	obtained_filename_after = gtk_source_style_scheme_get_filename (scheme);
	g_assert_cmpstr (obtained_filename_after, !=, obtained_filename_before);

	expected_filename = g_build_filename (dataset_dir, "classic.xml", NULL);
	g_assert_cmpstr (obtained_filename_after, ==, expected_filename);

	g_object_unref (manager);
	g_free (obtained_filename_before);
	g_free (dataset_dir);
	g_free (expected_filename);
}

static void
test_unknown_parent (void)
{
	GtkSourceStyleSchemeManager *manager;
	gchar *dataset_dir;
	GList *schemes;

	manager = gtk_source_style_scheme_manager_new ();

	dataset_dir = g_test_build_filename (G_TEST_DIST, "datasets", "style-schemes", "unknown-parent", NULL);
	set_single_search_path (manager, dataset_dir);
	g_free (dataset_dir);

	g_test_expect_message (G_LOG_DOMAIN,
			       G_LOG_LEVEL_WARNING,
			       "*Unknown parent-scheme*");
	schemes = gtk_source_style_scheme_manager_get_schemes (manager);
	g_test_assert_expected_messages ();

	g_assert_cmpuint (g_list_length (schemes), ==, 0);
	g_object_unref (manager);
}

static void
test_parents_ref_cycle (void)
{
	GtkSourceStyleSchemeManager *manager;
	gchar *dataset_dir;
	GList *schemes;

	manager = gtk_source_style_scheme_manager_new ();

	dataset_dir = g_test_build_filename (G_TEST_DIST, "datasets", "style-schemes", "parents-ref-cycle", NULL);
	set_single_search_path (manager, dataset_dir);
	g_free (dataset_dir);

	g_test_expect_message (G_LOG_DOMAIN,
			       G_LOG_LEVEL_WARNING,
			       "*parent-scheme reference cycle*");
	schemes = gtk_source_style_scheme_manager_get_schemes (manager);
	g_test_assert_expected_messages ();

	g_assert_cmpuint (g_list_length (schemes), ==, 0);
	g_object_unref (manager);
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
	g_test_add_func ("/StyleSchemeManager/unknown_parent", test_unknown_parent);
	g_test_add_func ("/StyleSchemeManager/parents_ref_cycle", test_parents_ref_cycle);

	ret = g_test_run ();
	gtk_source_finalize ();

	return ret;
}
