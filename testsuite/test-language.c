/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*- */
/*
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2013 - Paolo Borelli
 * Copyright (C) 2020 - Sébastien Wilmet
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

#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

typedef struct _TestFixture TestFixture;

struct _TestFixture {
	GtkSourceLanguageManager *manager;
};

/* If we are running from the source dir (e.g. during make check) we override
 * the path to read from the data dir.
 */
static void
test_fixture_setup (TestFixture   *fixture,
                    gconstpointer  data)
{
	gchar *dir;
	gchar **lang_dirs;

	dir = g_build_filename (TOP_SRCDIR, "data", "language-specs", NULL);

	fixture->manager = gtk_source_language_manager_get_default ();

	if (g_file_test (dir, G_FILE_TEST_IS_DIR))
	{
		lang_dirs = g_new0 (gchar *, 3);
		lang_dirs[0] = dir;
		lang_dirs[1] = g_test_build_filename (G_TEST_DIST, "language-specs", NULL);
	}
	else
	{
		const gchar * const *current;
		int i;

		g_free (dir);

		current = gtk_source_language_manager_get_search_path (fixture->manager);
		lang_dirs = g_new0 (gchar *, g_strv_length ((gchar **)current) + 2);
		for (i = 0; current[i] != NULL; i++)
		{
			lang_dirs[i] = g_strdup (current[i]);
		}
		lang_dirs[i] = g_test_build_filename (G_TEST_DIST, "language-specs", NULL);
	}

	gtk_source_language_manager_set_search_path (fixture->manager, lang_dirs);
	g_strfreev (lang_dirs);
}

static void
test_fixture_teardown (TestFixture   *fixture,
                       gconstpointer  data)
{
}

static void
check_strv_equal (const gchar **strv,
		  const gchar **expected_strv)
{
	if (strv == NULL || expected_strv == NULL)
	{
		g_assert (strv == NULL && expected_strv == NULL);
	}
	else
	{
		g_assert (g_strv_equal (strv, expected_strv));
	}
}

static gint
sort_strv_compare_cb (gconstpointer a,
		      gconstpointer b,
		      gpointer      user_data)
{
	const gchar * const *pointer_to_str_a = a;
	const gchar * const *pointer_to_str_b = b;

	g_assert (pointer_to_str_a != NULL);
	g_assert (pointer_to_str_b != NULL);
	g_assert (*pointer_to_str_a != NULL);
	g_assert (*pointer_to_str_b != NULL);

	return g_strcmp0 (*pointer_to_str_a, *pointer_to_str_b);
}

static void
sort_strv (gchar **strv)
{
	if (strv == NULL || strv[0] == NULL)
	{
		return;
	}

	g_qsort_with_data (strv,
			   g_strv_length (strv),
			   sizeof (gchar *),
			   sort_strv_compare_cb,
			   NULL);
}

static void
check_language (GtkSourceLanguage  *language,
		const gchar        *id,
		const gchar        *expected_name,
		const gchar        *expected_section,
		gboolean            expected_hidden,
		const gchar        *expected_extra_meta,
		const gchar       **expected_mime,
		const gchar       **expected_glob,
		const gchar       **expected_sorted_style_ids,
		const gchar        *style_id,
		const gchar        *expected_style_name)
{
	gchar **mime;
	gchar **glob;
	gchar **style_ids;

	g_assert_cmpstr (gtk_source_language_get_id (language), ==, id);
	g_assert_cmpstr (gtk_source_language_get_name (language), ==, expected_name);
	g_assert_cmpstr (gtk_source_language_get_section (language), ==, expected_section);
	g_assert (gtk_source_language_get_hidden (language) == expected_hidden);
	g_assert_cmpstr (gtk_source_language_get_metadata (language, "extra-meta"), ==, expected_extra_meta);

	/* Should be in the same order. */
	mime = gtk_source_language_get_mime_types (language);
	check_strv_equal ((const gchar **) mime, expected_mime);
	g_strfreev (mime);

	/* Should be in the same order. */
	glob = gtk_source_language_get_globs (language);
	check_strv_equal ((const gchar **) glob, expected_glob);
	g_strfreev (glob);

	/* Can be in a different order. */
	style_ids = gtk_source_language_get_style_ids (language);
	sort_strv (style_ids);
	check_strv_equal ((const gchar **) style_ids, expected_sorted_style_ids);
	g_strfreev (style_ids);

	if (expected_style_name != NULL)
	{
		g_assert_cmpstr (gtk_source_language_get_style_name (language, style_id), ==, expected_style_name);
	}
}

static void
test_language (TestFixture   *fixture,
               gconstpointer  data)
{
	GtkSourceLanguage *language;
	const gchar *mime[] = { "text/x-test", "application/x-test", NULL };
	const gchar *glob[] = { "*.test", "*.tst", NULL };
	const gchar *sorted_style_ids[] = { "test-full:keyword", "test-full:string", NULL };

	language = gtk_source_language_manager_get_language (fixture->manager, "test-full");
	check_language (language, "test-full", "Test Full", "Sources", FALSE, "extra", mime, glob, sorted_style_ids, "test-full:string", "String");

	language = gtk_source_language_manager_get_language (fixture->manager, "test-empty");
	check_language (language, "test-empty", "Test Empty", "Others", TRUE, NULL, NULL, NULL, NULL, NULL, NULL);
}

int
main (int    argc,
      char **argv)
{
	gtk_test_init (&argc, &argv);

	g_test_add ("/Language/language-properties", TestFixture, NULL, test_fixture_setup, test_language, test_fixture_teardown);

	return g_test_run();
}
