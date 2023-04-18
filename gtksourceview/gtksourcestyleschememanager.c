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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtksourcestyleschememanager.h"
#include "gtksourcestylescheme.h"
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

	/* Store the IDs of the available schemes.
	 * Because gtk_source_style_scheme_manager_get_scheme_ids() has a
	 * (transfer none) return value.
	 */
	gchar **ids;

	guint need_reload : 1;
};

enum
{
	PROP_0,
	PROP_SEARCH_PATH,
	PROP_SCHEME_IDS,
	N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES];
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
gtk_source_style_scheme_manager_get_property (GObject    *object,
					      guint       prop_id,
					      GValue     *value,
					      GParamSpec *pspec)
{
	GtkSourceStyleSchemeManager *manager = GTK_SOURCE_STYLE_SCHEME_MANAGER (object);

	switch (prop_id)
	{
		case PROP_SEARCH_PATH:
			g_value_set_boxed (value, gtk_source_style_scheme_manager_get_search_path (manager));
			break;

		case PROP_SCHEME_IDS:
			g_value_set_boxed (value, gtk_source_style_scheme_manager_get_scheme_ids (manager));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gtk_source_style_scheme_manager_set_property (GObject 	   *object,
					      guint         prop_id,
					      const GValue *value,
					      GParamSpec   *pspec)
{
	GtkSourceStyleSchemeManager *manager = GTK_SOURCE_STYLE_SCHEME_MANAGER (object);

	switch (prop_id)
	{
		case PROP_SEARCH_PATH:
			gtk_source_style_scheme_manager_set_search_path (manager, g_value_get_boxed (value));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
free_schemes (GtkSourceStyleSchemeManager *manager)
{
	if (manager->priv->schemes_hash_table != NULL)
	{
		g_hash_table_destroy (manager->priv->schemes_hash_table);
		manager->priv->schemes_hash_table = NULL;
	}

	g_strfreev (manager->priv->ids);
	manager->priv->ids = NULL;
}

static void
gtk_source_style_scheme_manager_finalize (GObject *object)
{
	GtkSourceStyleSchemeManager *manager = GTK_SOURCE_STYLE_SCHEME_MANAGER (object);

	g_strfreev (manager->priv->search_path);
	free_schemes (manager);

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

	object_class->get_property = gtk_source_style_scheme_manager_get_property;
	object_class->set_property = gtk_source_style_scheme_manager_set_property;
	object_class->finalize = gtk_source_style_scheme_manager_finalize;

	/**
	 * GtkSourceStyleSchemeManager:search-path:
	 *
	 * List of directories and files where the style schemes are located.
	 *
	 * Note that a path to a single file is accepted too, not just
	 * directories.
	 *
	 * To load the style schemes from the filesystem,
	 * #GtkSourceStyleSchemeManager first looks at the first path, then the
	 * second path, etc. If there are duplicates (same style scheme ID), the
	 * first encountered one has the priority.
	 *
	 * So the list of paths must be set in priority order, from highest to
	 * lowest.
	 */
	properties[PROP_SEARCH_PATH] =
		g_param_spec_boxed ("search-path",
				    "search-path",
				    "",
				    G_TYPE_STRV,
				    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

	/**
	 * GtkSourceStyleSchemeManager:scheme-ids:
	 *
	 * List of the IDs of the available style schemes.
	 */
	properties[PROP_SCHEME_IDS] =
		g_param_spec_boxed ("scheme-ids",
				    "scheme-ids",
				    "",
				    G_TYPE_STRV,
				    G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

	g_object_class_install_properties (object_class, N_PROPERTIES, properties);
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

/* Returns: (transfer full) */
static GQueue *
get_parents_chain (GHashTable           *schemes_hash_table,
		   GtkSourceStyleScheme *scheme,
		   gboolean             *ok)
{
	GQueue *chain = g_queue_new ();
	GtkSourceStyleScheme *cur_scheme = scheme;

	*ok = TRUE;

	while (TRUE)
	{
		const gchar *parent_id;
		GtkSourceStyleScheme *parent_scheme;

		g_queue_push_tail (chain, g_object_ref (cur_scheme));

		parent_id = _gtk_source_style_scheme_get_parent_id (cur_scheme);
		if (parent_id == NULL)
		{
			/* No parents */
			break;
		}

		parent_scheme = g_hash_table_lookup (schemes_hash_table, parent_id);
		if (parent_scheme == NULL)
		{
			g_warning ("Unknown parent-scheme '%s' in scheme '%s'.",
				   parent_id,
				   gtk_source_style_scheme_get_id (cur_scheme));
			*ok = FALSE;
			break;
		}

		if (g_queue_find (chain, parent_scheme) != NULL)
		{
			g_warning ("parent-scheme reference cycle for scheme '%s'.",
				   gtk_source_style_scheme_get_id (scheme));
			*ok = FALSE;
			break;
		}

		cur_scheme = parent_scheme;
	}

	return chain;
}

static void
chain_parents (GQueue *chain)
{
	GList *child_node = chain->head;

	while (child_node != NULL)
	{
		GtkSourceStyleScheme *child_scheme = child_node->data;
		GList *parent_node = child_node->next;

		if (parent_node != NULL)
		{
			GtkSourceStyleScheme *parent_scheme = parent_node->data;
			_gtk_source_style_scheme_set_parent (child_scheme, parent_scheme);
		}

		child_node = parent_node;
	}
}

static void
remove_chain_from_schemes_hash_table (GtkSourceStyleSchemeManager *manager,
				      GQueue                      *chain)
{
	GList *l;

	for (l = chain->head; l != NULL; l = l->next)
	{
		GtkSourceStyleScheme *scheme = l->data;
		const gchar *id = gtk_source_style_scheme_get_id (scheme);

		g_hash_table_remove (manager->priv->schemes_hash_table, id);
	}
}

static gboolean
do_setup_parent_schemes (GtkSourceStyleSchemeManager *manager)
{
	GList *schemes;
	GList *l;
	gboolean ok = TRUE;

	schemes = g_hash_table_get_values (manager->priv->schemes_hash_table);

	for (l = schemes; l != NULL && ok; l = l->next)
	{
		GtkSourceStyleScheme *cur_scheme = l->data;
		GQueue *chain;

		chain = get_parents_chain (manager->priv->schemes_hash_table, cur_scheme, &ok);

		if (ok)
		{
			chain_parents (chain);
		}
		else
		{
			remove_chain_from_schemes_hash_table (manager, chain);
		}

		g_queue_free_full (chain, g_object_unref);
	}

	g_list_free (schemes);
	return ok;
}

static void
setup_parent_schemes (GtkSourceStyleSchemeManager *manager)
{
	while (!do_setup_parent_schemes (manager))
		; /* empty loop body */
}

static gint
schemes_compare_by_name (GtkSourceStyleScheme *scheme1,
			 GtkSourceStyleScheme *scheme2)
{
	const gchar *name1 = gtk_source_style_scheme_get_name (scheme1);
	const gchar *name2 = gtk_source_style_scheme_get_name (scheme2);

	return g_utf8_collate (name1, name2);
}

static void
update_scheme_ids (GtkSourceStyleSchemeManager *manager)
{
	GPtrArray *ids;
	GList *schemes;
	GList *l;

	ids = g_ptr_array_new ();

	schemes = g_hash_table_get_values (manager->priv->schemes_hash_table);
	schemes = g_list_sort (schemes, (GCompareFunc) schemes_compare_by_name);

	for (l = schemes; l != NULL; l = l->next)
	{
		const gchar *cur_id = gtk_source_style_scheme_get_id (l->data);
		g_ptr_array_add (ids, g_strdup (cur_id));
	}

	g_ptr_array_add (ids, NULL);
	manager->priv->ids = (gchar **) g_ptr_array_free (ids, FALSE);

	g_list_free (schemes);
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

	free_schemes (manager);
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

	setup_parent_schemes (manager);
	update_scheme_ids (manager);
	manager->priv->need_reload = FALSE;

	g_slist_free_full (files, g_free);
}

static void
search_path_changed (GtkSourceStyleSchemeManager *manager)
{
	manager->priv->need_reload = TRUE;

	g_object_notify_by_pspec (G_OBJECT (manager), properties[PROP_SEARCH_PATH]);
	g_object_notify_by_pspec (G_OBJECT (manager), properties[PROP_SCHEME_IDS]);
}

/**
 * gtk_source_style_scheme_manager_set_search_path:
 * @manager: a #GtkSourceStyleSchemeManager.
 * @path: (array zero-terminated=1) (nullable): a %NULL-terminated array of
 *   strings, or %NULL to reset the search path to its default value.
 *
 * Sets the #GtkSourceStyleSchemeManager:search-path property.
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

	search_path_changed (manager);
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
 * Adds @path at the end of the #GtkSourceStyleSchemeManager:search-path.
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

	search_path_changed (manager);
}

/**
 * gtk_source_style_scheme_manager_prepend_search_path:
 * @manager: a #GtkSourceStyleSchemeManager.
 * @path: a directory or a filename.
 *
 * Prepends @path to the #GtkSourceStyleSchemeManager:search-path.
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

	search_path_changed (manager);
}

/**
 * gtk_source_style_scheme_manager_get_search_path:
 * @manager: a #GtkSourceStyleSchemeManager.
 *
 * Returns: (array zero-terminated=1) (transfer none): the value of the
 *   #GtkSourceStyleSchemeManager:search-path property.
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

	g_object_notify_by_pspec (G_OBJECT (manager), properties[PROP_SCHEME_IDS]);
}

/**
 * gtk_source_style_scheme_manager_get_scheme_ids:
 * @manager: a #GtkSourceStyleSchemeManager.
 *
 * Returns: (nullable) (array zero-terminated=1) (transfer none): a
 *   %NULL-terminated array of strings containing the IDs of the available style
 *   schemes, or %NULL if no style schemes are available. The array is sorted
 *   alphabetically according to the scheme name.
 */
const gchar * const *
gtk_source_style_scheme_manager_get_scheme_ids (GtkSourceStyleSchemeManager *manager)
{
	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME_MANAGER (manager), NULL);

	reload_if_needed (manager);

	return (const gchar * const *) manager->priv->ids;
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
