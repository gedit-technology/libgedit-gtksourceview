/* SPDX-FileCopyrightText: 2023 - Sébastien Wilmet <swilmet@gnome.org>
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <gtksourceview/gtksource.h>
#include <stdlib.h>

/* This program prints, in HTML, the names of all GtkSourceLanguages found,
 * categorized by section. With the "Other" section printed last.
 */

#define LAST_SECTION "Other"

static void
set_single_search_path (GtkSourceLanguageManager *language_manager,
			const gchar              *path)
{
	const gchar *search_path[] = { path, NULL };

	gtk_source_language_manager_set_search_path (language_manager, (gchar **) search_path);
}

static void
queue_free (gpointer queue)
{
	if (queue != NULL)
	{
		g_queue_free_full (queue, g_object_unref);
	}
}

static gint
compare_sections (gconstpointer a,
		  gconstpointer b)
{
	const gchar *str_a = a;
	const gchar *str_b = b;

	return g_ascii_strcasecmp (str_a, str_b);
}

static gint
compare_languages (gconstpointer a,
		   gconstpointer b,
		   gpointer      user_data)
{
	GtkSourceLanguage *language_a = GTK_SOURCE_LANGUAGE (a);
	GtkSourceLanguage *language_b = GTK_SOURCE_LANGUAGE (b);
	const gchar *name_a = gtk_source_language_get_name (language_a);
	const gchar *name_b = gtk_source_language_get_name (language_b);

	return g_ascii_strcasecmp (name_a, name_b);
}

static void
insert_language_into_hash_table (GHashTable        *hash_table,
				 GtkSourceLanguage *language)
{
	const gchar *section;
	GQueue *queue;

	section = gtk_source_language_get_section (language);
	if (section == NULL)
	{
		g_error ("Language without section.");
	}

	queue = g_hash_table_lookup (hash_table, section);

	if (queue == NULL)
	{
		queue = g_queue_new ();
		g_hash_table_insert (hash_table, g_strdup (section), queue);
	}

	g_queue_insert_sorted (queue,
			       g_object_ref (language),
			       compare_languages,
			       NULL);
}

static GHashTable *
get_languages_hash_table (GtkSourceLanguageManager *language_manager)
{
	/* Keys: "owned gchar*": GtkSourceLanguage section.
	 * Values: "owned GQueue*": sorted queue of owned GtkSourceLanguage's.
	 */
	GHashTable *hash_table;
	const gchar * const *language_ids;
	gint i;

	hash_table = g_hash_table_new_full (g_str_hash,
					    g_str_equal,
					    g_free,
					    queue_free);

	language_ids = gtk_source_language_manager_get_language_ids (language_manager);
	if (language_ids == NULL)
	{
		return hash_table;
	}

	for (i = 0; language_ids[i] != NULL; i++)
	{
		GtkSourceLanguage *cur_language;

		cur_language = gtk_source_language_manager_get_language (language_manager, language_ids[i]);

		if (!gtk_source_language_get_hidden (cur_language))
		{
			insert_language_into_hash_table (hash_table, cur_language);
		}
	}

	return hash_table;
}

/* Returns: (transfer container). */
static GList *
get_sections_list (GHashTable *hash_table)
{
	GList *list;

	list = g_hash_table_get_keys (hash_table);
	list = g_list_sort (list, compare_sections);

	return list;
}

static void
print_section (GHashTable  *hash_table,
	       const gchar *section)
{
	GQueue *queue;
	gchar *section_escaped;
	GList *l;

	queue = g_hash_table_lookup (hash_table, section);
	if (queue == NULL)
	{
		return;
	}

	section_escaped = g_markup_escape_text (section, -1);
	g_print ("<h2>%s</h2>\n", section_escaped);
	g_free (section_escaped);

	g_print ("<ul>\n");

	for (l = queue->head; l != NULL; l = l->next)
	{
		GtkSourceLanguage *cur_language = GTK_SOURCE_LANGUAGE (l->data);
		const gchar *name;
		gchar *escaped_name;

		name = gtk_source_language_get_name (cur_language);
		escaped_name = g_markup_escape_text (name, -1);

		g_print ("  <li>%s</li>\n", escaped_name);

		g_free (escaped_name);
	}

	g_print ("</ul>\n");
}

int
main (int    argc,
      char **argv)
{
	const gchar *language_specs_dir;
	GtkSourceLanguageManager *language_manager;
	GHashTable *hash_table;
	GList *sections_list;
	GList *l;

	if (argc != 2)
	{
		g_printerr ("Usage: %s <language-specs-dir>\n", argv[0]);
		return EXIT_FAILURE;
	}

	gtk_source_init ();

	language_specs_dir = argv[1];

	language_manager = gtk_source_language_manager_new ();
	set_single_search_path (language_manager, language_specs_dir);

	hash_table = get_languages_hash_table (language_manager);

	sections_list = get_sections_list (hash_table);
	for (l = sections_list; l != NULL; l = l->next)
	{
		const gchar *section = l->data;

		if (g_strcmp0 (section, LAST_SECTION) != 0)
		{
			print_section (hash_table, section);
			g_print ("\n");
		}
	}
	g_list_free (sections_list);

	print_section (hash_table, LAST_SECTION);

	g_object_unref (language_manager);
	g_hash_table_unref (hash_table);
	gtk_source_finalize ();
	return EXIT_SUCCESS;
}
