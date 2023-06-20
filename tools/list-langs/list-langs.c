/* SPDX-FileCopyrightText: 2023 - Sébastien Wilmet <swilmet@gnome.org>
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <gtksourceview/gtksource.h>
#include <stdlib.h>

static void
set_single_search_path (GtkSourceLanguageManager *language_manager,
			const gchar              *path)
{
	const gchar *search_path[] = { path, NULL };

	gtk_source_language_manager_set_search_path (language_manager, (gchar **) search_path);
}

int
main (int    argc,
      char **argv)
{
	const gchar *language_specs_dir;
	GtkSourceLanguageManager *language_manager;
	const gchar * const *language_ids;
	gint i;

	if (argc != 2)
	{
		g_printerr ("Usage: %s <language-specs-dir>\n", argv[0]);
		return EXIT_FAILURE;
	}

	gtk_source_init ();

	language_specs_dir = argv[1];

	language_manager = gtk_source_language_manager_new ();
	set_single_search_path (language_manager, language_specs_dir);

	language_ids = gtk_source_language_manager_get_language_ids (language_manager);
	if (language_ids == NULL)
	{
		goto end;
	}

	g_print ("<ul>\n");

	for (i = 0; language_ids[i] != NULL; i++)
	{
		GtkSourceLanguage *cur_language;

		cur_language = gtk_source_language_manager_get_language (language_manager, language_ids[i]);

		if (!gtk_source_language_get_hidden (cur_language))
		{
			const gchar *name;
			gchar *escaped_name;

			name = gtk_source_language_get_name (cur_language);
			escaped_name = g_markup_escape_text (name, -1);

			g_print ("  <li>%s</li>\n", escaped_name);

			g_free (escaped_name);
		}
	}

	g_print ("</ul>\n");

end:
	g_object_unref (language_manager);
	gtk_source_finalize ();
	return EXIT_SUCCESS;
}
