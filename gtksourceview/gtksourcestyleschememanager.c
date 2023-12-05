/*
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2003-2007 - Paolo Maggi <paolo@gnome.org>
 * Copyright (C) 2023 - Sébastien Wilmet <swilmet@gnome.org>
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

#include "gtksourcestyleschememanager.h"
#include "gtksourceutils-private.h"

/**
 * SECTION:styleschememanager
 * @Title: GtkSourceStyleSchemeManager
 * @Short_description: Provides access to #GtkSourceStyleScheme's
 *
 * Object which provides access to #GtkSourceStyleScheme's.
 */

struct _GtkSourceStyleSchemeManagerPrivate
{
	gchar **search_path;

	/* Key: scheme ID (owned gchar*)
	 * Value: owned GtkSourceStyleScheme*
	 */
	GHashTable *schemes_hash_table;

	guint need_reload : 1;
};

enum
{
	SIGNAL_CHANGED,
	N_SIGNALS
};

static guint signals[N_SIGNALS];
static GtkSourceStyleSchemeManager *default_instance;

#define STYLES_DIR		"styles"
#define SCHEME_FILE_SUFFIX	".xml"

G_DEFINE_TYPE_WITH_PRIVATE (GtkSourceStyleSchemeManager, gtk_source_style_scheme_manager, G_TYPE_OBJECT)

static void
init_default_search_path (GtkSourceStyleSchemeManager *manager)
{
	g_strfreev (manager->priv->search_path);
	manager->priv->search_path = _gtk_source_utils_get_default_dirs (STYLES_DIR);
}

static void
gtk_source_style_scheme_manager_finalize (GObject *object)
{
	GtkSourceStyleSchemeManager *manager = GTK_SOURCE_STYLE_SCHEME_MANAGER (object);

	g_strfreev (manager->priv->search_path);

	if (manager->priv->schemes_hash_table != NULL)
	{
		g_hash_table_destroy (manager->priv->schemes_hash_table);
	}

	if (default_instance == manager)
	{
		default_instance = NULL;
	}

	G_OBJECT_CLASS (gtk_source_style_scheme_manager_parent_class)->finalize (object);
}

static void
gtk_source_style_scheme_manager_class_init (GtkSourceStyleSchemeManagerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = gtk_source_style_scheme_manager_finalize;

	/**
	 * GtkSourceStyleSchemeManager::changed:
	 * @manager: the #GtkSourceStyleSchemeManager emitting the signal.
	 *
	 * Emitted when the @manager's content has changed.
	 *
	 * Since: 299.0
	 */
	signals[SIGNAL_CHANGED] =
		g_signal_new ("changed",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_FIRST,
			      0, NULL, NULL, NULL,
			      G_TYPE_NONE, 0);
}

static void
gtk_source_style_scheme_manager_init (GtkSourceStyleSchemeManager *manager)
{
	manager->priv = gtk_source_style_scheme_manager_get_instance_private (manager);
	manager->priv->need_reload = TRUE;
}

/**
 * gtk_source_style_scheme_manager_new:
 *
 * Note: if you don't need more than one style manager, then
 * gtk_source_style_scheme_manager_get_default() is more convenient to use.
 *
 * Returns: a new #GtkSourceStyleSchemeManager.
 */
GtkSourceStyleSchemeManager *
gtk_source_style_scheme_manager_new (void)
{
	return g_object_new (GTK_SOURCE_TYPE_STYLE_SCHEME_MANAGER, NULL);
}

/**
 * gtk_source_style_scheme_manager_get_default:
 *
 * Returns: (transfer none): the default #GtkSourceStyleSchemeManager instance.
 */
GtkSourceStyleSchemeManager *
gtk_source_style_scheme_manager_get_default (void)
{
	if (default_instance == NULL)
	{
		default_instance = gtk_source_style_scheme_manager_new ();
	}

	return default_instance;
}

GtkSourceStyleSchemeManager *
_gtk_source_style_scheme_manager_peek_default (void)
{
	return default_instance;
}

static void
reload_if_needed (GtkSourceStyleSchemeManager *manager)
{
	GSList *files;
	GSList *l;

	if (!manager->priv->need_reload)
	{
		return;
	}

	if (manager->priv->schemes_hash_table != NULL)
	{
		g_hash_table_destroy (manager->priv->schemes_hash_table);
	}

	manager->priv->schemes_hash_table = g_hash_table_new_full (g_str_hash,
								   g_str_equal,
								   g_free,
								   g_object_unref);

	files = _gtk_source_utils_get_file_list ((gchar **)gtk_source_style_scheme_manager_get_search_path (manager),
						 SCHEME_FILE_SUFFIX,
						 FALSE);

	for (l = files; l != NULL; l = l->next)
	{
		const gchar *filename = l->data;
		GtkSourceStyleScheme *scheme;
		const gchar *id;

		scheme = _gtk_source_style_scheme_new_from_file (filename);
		if (scheme == NULL)
		{
			continue;
		}

		id = gtk_source_style_scheme_get_id (scheme);

		/* A style scheme with the same ID has already been loaded. This
		 * style scheme has a lower priority, since it comes later.
		 */
		if (g_hash_table_contains (manager->priv->schemes_hash_table, id))
		{
			g_object_unref (scheme);
			continue;
		}

		g_hash_table_insert (manager->priv->schemes_hash_table,
				     g_strdup (id),
				     scheme);
	}

	manager->priv->need_reload = FALSE;

	g_slist_free_full (files, g_free);
}

/**
 * gtk_source_style_scheme_manager_set_search_path:
 * @manager: a #GtkSourceStyleSchemeManager.
 * @path: (array zero-terminated=1) (nullable): a %NULL-terminated array of
 *   strings, or %NULL to reset the search path to its default value.
 *
 * Sets the search path of @manager.
 *
 * @manager will then try to load style schemes from the locations provided in
 * the search path.
 *
 * The search path can contain:
 * - Paths to directories;
 * - Paths to individual files.
 *
 * To load the style schemes from the filesystem, #GtkSourceStyleSchemeManager
 * first looks at the first path, then the second path, etc. If there are
 * duplicates (same #GtkSourceStyleScheme ID), the first encountered one has the
 * priority.
 *
 * So the list of paths must be set in priority order, from highest to lowest.
 */
void
gtk_source_style_scheme_manager_set_search_path (GtkSourceStyleSchemeManager  *manager,
						 gchar	                     **path)
{
	gchar **path_dup;

	g_return_if_fail (GTK_SOURCE_IS_STYLE_SCHEME_MANAGER (manager));

	if (path == NULL)
	{
		init_default_search_path (manager);
		return;
	}

	/* The order is important, as @path could be equal to priv->search_path. */
	path_dup = g_strdupv (path);
	g_strfreev (manager->priv->search_path);
	manager->priv->search_path = path_dup;

	gtk_source_style_scheme_manager_force_rescan (manager);
}

static void
add_search_path_to_ptr_array (GtkSourceStyleSchemeManager *manager,
			      GPtrArray                   *ptr_array)
{
	const gchar * const *search_path;
	gint i;

	search_path = gtk_source_style_scheme_manager_get_search_path (manager);
	if (search_path == NULL)
	{
		return;
	}

	for (i = 0; search_path[i] != NULL; i++)
	{
		g_ptr_array_add (ptr_array, g_strdup (search_path[i]));
	}
}

/**
 * gtk_source_style_scheme_manager_append_search_path:
 * @manager: a #GtkSourceStyleSchemeManager.
 * @path: a directory or a filename.
 *
 * Adds @path at the end of the @manager's search path (i.e., at the lowest
 * priority). See gtk_source_style_scheme_manager_set_search_path().
 */
void
gtk_source_style_scheme_manager_append_search_path (GtkSourceStyleSchemeManager *manager,
						    const gchar                 *path)
{
	GPtrArray *new_search_path;

	g_return_if_fail (GTK_SOURCE_IS_STYLE_SCHEME_MANAGER (manager));
	g_return_if_fail (path != NULL);

	new_search_path = g_ptr_array_new ();
	add_search_path_to_ptr_array (manager, new_search_path);
	g_ptr_array_add (new_search_path, g_strdup (path));
	g_ptr_array_add (new_search_path, NULL);

	g_strfreev (manager->priv->search_path);
	manager->priv->search_path = (gchar **) g_ptr_array_free (new_search_path, FALSE);

	gtk_source_style_scheme_manager_force_rescan (manager);
}

/**
 * gtk_source_style_scheme_manager_prepend_search_path:
 * @manager: a #GtkSourceStyleSchemeManager.
 * @path: a directory or a filename.
 *
 * Adds @path at the beginning of the @manager's search path (i.e., at the
 * highest priority). See gtk_source_style_scheme_manager_set_search_path().
 */
void
gtk_source_style_scheme_manager_prepend_search_path (GtkSourceStyleSchemeManager *manager,
						     const gchar                 *path)
{
	GPtrArray *new_search_path;

	g_return_if_fail (GTK_SOURCE_IS_STYLE_SCHEME_MANAGER (manager));
	g_return_if_fail (path != NULL);

	new_search_path = g_ptr_array_new ();
	g_ptr_array_add (new_search_path, g_strdup (path));
	add_search_path_to_ptr_array (manager, new_search_path);
	g_ptr_array_add (new_search_path, NULL);

	g_strfreev (manager->priv->search_path);
	manager->priv->search_path = (gchar **) g_ptr_array_free (new_search_path, FALSE);

	gtk_source_style_scheme_manager_force_rescan (manager);
}

/**
 * gtk_source_style_scheme_manager_get_search_path:
 * @manager: a #GtkSourceStyleSchemeManager.
 *
 * Gets the search path of @manager.
 *
 * See also gtk_source_style_scheme_manager_set_search_path().
 *
 * Returns: (array zero-terminated=1) (transfer none): the search path of
 *   @manager.
 */
const gchar * const *
gtk_source_style_scheme_manager_get_search_path (GtkSourceStyleSchemeManager *manager)
{
	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME_MANAGER (manager), NULL);

	if (manager->priv->search_path == NULL)
	{
		init_default_search_path (manager);
	}

	return (const gchar * const *) manager->priv->search_path;
}

/**
 * gtk_source_style_scheme_manager_force_rescan:
 * @manager: a #GtkSourceStyleSchemeManager.
 *
 * Marks any currently cached information about the available style schemes as
 * invalid. All the available style schemes will be reloaded next time the
 * @manager is accessed.
 */
void
gtk_source_style_scheme_manager_force_rescan (GtkSourceStyleSchemeManager *manager)
{
	g_return_if_fail (GTK_SOURCE_IS_STYLE_SCHEME_MANAGER (manager));

	manager->priv->need_reload = TRUE;
	g_signal_emit (manager, signals[SIGNAL_CHANGED], 0);
}

static gint
schemes_compare_by_name (GtkSourceStyleScheme *scheme1,
			 GtkSourceStyleScheme *scheme2)
{
	const gchar *name1 = gtk_source_style_scheme_get_name (scheme1);
	const gchar *name2 = gtk_source_style_scheme_get_name (scheme2);

	return g_utf8_collate (name1, name2);
}

/**
 * gtk_source_style_scheme_manager_get_schemes:
 * @manager: a #GtkSourceStyleSchemeManager.
 *
 * Returns: (transfer container) (element-type GtkSourceStyleScheme): a list of
 *   all the #GtkSourceStyleScheme's that are part of @manager. The list is
 *   sorted alphabetically according to the scheme name.
 * Since: 299.0
 */
GList *
gtk_source_style_scheme_manager_get_schemes (GtkSourceStyleSchemeManager *manager)
{
	GList *schemes;

	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME_MANAGER (manager), NULL);

	reload_if_needed (manager);

	schemes = g_hash_table_get_values (manager->priv->schemes_hash_table);
	return g_list_sort (schemes, (GCompareFunc) schemes_compare_by_name);
}

/**
 * gtk_source_style_scheme_manager_get_scheme:
 * @manager: a #GtkSourceStyleSchemeManager.
 * @scheme_id: style scheme ID to find.
 *
 * Returns: (transfer none) (nullable): the #GtkSourceStyleScheme object
 *   corresponding to @scheme_id, or %NULL if not found.
 */
GtkSourceStyleScheme *
gtk_source_style_scheme_manager_get_scheme (GtkSourceStyleSchemeManager *manager,
					    const gchar                 *scheme_id)
{
	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME_MANAGER (manager), NULL);
	g_return_val_if_fail (scheme_id != NULL, NULL);

	reload_if_needed (manager);

	return g_hash_table_lookup (manager->priv->schemes_hash_table, scheme_id);
}
