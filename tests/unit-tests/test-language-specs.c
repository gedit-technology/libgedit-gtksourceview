/*
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2022 - Sébastien Wilmet <swilmet@gnome.org>
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

#define DEBUG FALSE

static GtkSourceLanguageManager *
get_language_manager (void)
{
	GtkSourceLanguageManager *language_manager;
	GFile *lang_specs_srcdir;
	GFileType file_type;

	language_manager = gtk_source_language_manager_get_default ();

	/* If the unit test program is running from the source directory (e.g.
	 * with `make check`), load only the language specs located there, not
	 * the installed ones.
	 * For better reproducibility, and the ability to run the unit test
	 * without installing the module.
	 */
	lang_specs_srcdir = g_file_new_build_filename (TOP_SRCDIR, "data", "language-specs", NULL);
	file_type = g_file_query_file_type (lang_specs_srcdir, G_FILE_QUERY_INFO_NONE, NULL);
	if (file_type == G_FILE_TYPE_DIRECTORY)
	{
		const gchar *search_path[2];

		search_path[0] = g_file_peek_path (lang_specs_srcdir);
		search_path[1] = NULL;

#if DEBUG
		g_print ("%s: search language specs only in: %s\n", G_STRFUNC, search_path[0]);
#endif

		gtk_source_language_manager_set_search_path (language_manager,
							     (gchar **) search_path);
	}
	else
	{
#if DEBUG
		g_print ("%s: search language specs in default search path.\n", G_STRFUNC);
#endif
	}

	g_object_unref (lang_specs_srcdir);
	return language_manager;
}

static gboolean
skip_language (GtkSourceLanguage *language)
{
	const gchar *language_id;

	/* Hidden *.lang files are meant to be embedded into others, and as such
	 * may not contain everything.
	 */
	if (gtk_source_language_get_hidden (language))
	{
		return TRUE;
	}

	language_id = gtk_source_language_get_id (language);

	/* FIXME: these lang files need to be fixed. */
	if (g_str_equal (language_id, "cuda") ||
	    g_str_equal (language_id, "gdb-log") ||
	    g_str_equal (language_id, "llvm") ||
	    g_str_equal (language_id, "meson"))
	{
		return TRUE;
	}

	return FALSE;
}

static void
apply_language_to_buffer (GtkSourceLanguage *language)
{
	GtkSourceBuffer *buffer;

	buffer = gtk_source_buffer_new_with_language (language);
	g_object_unref (buffer);
}

static void
test_apply_to_buffer (void)
{
	GtkSourceLanguageManager *language_manager;
	const gchar * const *language_ids;
	gint i;

	language_manager = get_language_manager ();
	language_ids = gtk_source_language_manager_get_language_ids (language_manager);

	if (language_ids == NULL)
	{
		return;
	}

	for (i = 0; language_ids[i] != NULL; i++)
	{
		const gchar *cur_language_id = language_ids[i];
		GtkSourceLanguage *language;

		language = gtk_source_language_manager_get_language (language_manager,
								     cur_language_id);

		if (skip_language (language))
		{
#if DEBUG
			g_print ("SKIP language %s\n", cur_language_id);
#endif
		}
		else
		{
#if DEBUG
			g_print ("test language %s\n", cur_language_id);
#endif
			apply_language_to_buffer (language);
		}
	}
}

int
main (int    argc,
      char **argv)
{
	gint ret;

	gtk_test_init (&argc, &argv);
	gtk_source_init ();

	g_test_add_func ("/LanguageSpecs/apply_to_buffer", test_apply_to_buffer);

	ret = g_test_run ();
	gtk_source_finalize ();

	return ret;
}
