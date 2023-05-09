/*
 * This file is part of GtkSourceView
 *
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

#include "gtksourcestyleschemecss.h"

struct _GtkSourceStyleSchemeCss
{
	GtkSourceStyleScheme *scheme; /* weak ref */

	GtkCssProvider *main_provider;
	GtkCssProvider *cursors_provider;
};

/* We need to be lower than the application priority to allow application
 * overrides.
 */
#define GTK_SOURCE_STYLE_PROVIDER_PRIORITY (GTK_STYLE_PROVIDER_PRIORITY_APPLICATION - 1)

GtkSourceStyleSchemeCss *
_gtk_source_style_scheme_css_new (GtkSourceStyleScheme *scheme)
{
	GtkSourceStyleSchemeCss *scheme_css;

	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME (scheme), NULL);

	scheme_css = g_new0 (GtkSourceStyleSchemeCss, 1);

	g_set_weak_pointer (&scheme_css->scheme, scheme);
	scheme_css->main_provider = gtk_css_provider_new ();

	return scheme_css;
}

void
_gtk_source_style_scheme_css_free (GtkSourceStyleSchemeCss *scheme_css)
{
	if (scheme_css != NULL)
	{
		g_clear_weak_pointer (&scheme_css->scheme);
		g_clear_object (&scheme_css->main_provider);
		g_clear_object (&scheme_css->cursors_provider);
		g_free (scheme_css);
	}
}

static gboolean
get_style_foreground_color (GtkSourceStyleScheme *scheme,
			    const gchar          *style_id,
			    GdkRGBA              *foreground_color)
{
	GtkSourceStyle *style;
	GtkSourceStyleData *style_data;
	gboolean success = FALSE;

	g_return_val_if_fail (GTK_SOURCE_IS_STYLE_SCHEME (scheme), FALSE);
	g_return_val_if_fail (style_id != NULL, FALSE);
	g_return_val_if_fail (foreground_color != NULL, FALSE);

	style = gtk_source_style_scheme_get_style (scheme, style_id);
	if (style == NULL)
	{
		return FALSE;
	}

	style_data = gtk_source_style_get_data (style);

	if (style_data->use_foreground_color)
	{
		*foreground_color = style_data->foreground_color;
		success = TRUE;
	}

	g_free (style_data);
	return success;
}

static gchar *
get_cursors_css_style (GtkSourceStyleScheme *scheme,
		       GtkWidget            *widget)
{
	gboolean primary_color_set;
	gboolean secondary_color_set;
	GdkRGBA primary_color;
	GdkRGBA secondary_color;
	gchar *secondary_color_str;
	GString *css;

	primary_color_set = get_style_foreground_color (scheme, "cursor", &primary_color);
	secondary_color_set = get_style_foreground_color (scheme, "secondary-cursor", &secondary_color);

	if (!primary_color_set && !secondary_color_set)
	{
		return NULL;
	}

	css = g_string_new ("textview text {\n");

	if (primary_color_set)
	{
		gchar *primary_color_str;

		primary_color_str = gdk_rgba_to_string (&primary_color);
		g_string_append_printf (css,
					"\tcaret-color: %s;\n",
					primary_color_str);
		g_free (primary_color_str);
	}

	if (!secondary_color_set)
	{
		GtkStyleContext *context;
		GdkRGBA *background_color;

		g_assert (primary_color_set);

		context = gtk_widget_get_style_context (widget);

		gtk_style_context_save (context);
		gtk_style_context_set_state (context, GTK_STATE_FLAG_NORMAL);

		gtk_style_context_get (context,
				       gtk_style_context_get_state (context),
				       "background-color", &background_color,
				       NULL);

		gtk_style_context_restore (context);

		/* Blend primary cursor color with background color. */
		secondary_color.red = (primary_color.red + background_color->red) * 0.5;
		secondary_color.green = (primary_color.green + background_color->green) * 0.5;
		secondary_color.blue = (primary_color.blue + background_color->blue) * 0.5;
		secondary_color.alpha = (primary_color.alpha + background_color->alpha) * 0.5;

		gdk_rgba_free (background_color);
	}

	secondary_color_str = gdk_rgba_to_string (&secondary_color);
	g_string_append_printf (css,
				"\t-gtk-secondary-caret-color: %s;\n",
				secondary_color_str);
	g_free (secondary_color_str);

	g_string_append_printf (css, "}\n");

	return g_string_free (css, FALSE);
}

/* The GtkCssProvider for the cursors needs a GtkWidget to blend with the
 * background color in case the secondary cursor color isn't defined.
 *
 * The background color is either defined by the GtkSourceStyleScheme, or if
 * it's not defined it is taken from the GTK theme.
 *
 * So ideally, if the GTK theme changes at runtime, we should regenerate the
 * GtkCssProvider for the cursors, but it isn't done.
 */
static GtkCssProvider *
create_cursors_provider (GtkSourceStyleScheme *scheme,
			 GtkWidget            *widget)
{
	gchar *css;
	GtkCssProvider *provider;
	GError *error = NULL;

	css = get_cursors_css_style (scheme, widget);
	if (css == NULL)
	{
		return NULL;
	}

	provider = gtk_css_provider_new ();
	gtk_css_provider_load_from_data (provider, css, -1, &error);
	g_free (css);

	if (error != NULL)
	{
		g_warning ("Error when loading CSS for cursors: %s", error->message);
		g_clear_error (&error);
		g_clear_object (&provider);
	}

	return provider;
}

/* Adds the #GtkCssProvider's (that are part of @scheme_css) to @widget.
 * @widget is typically a #GtkSourceView.
 */
void
_gtk_source_style_scheme_css_apply (GtkSourceStyleSchemeCss *scheme_css,
				    GtkWidget               *widget)
{
	GtkStyleContext *context;

	g_return_if_fail (scheme_css != NULL);
	g_return_if_fail (GTK_IS_WIDGET (widget));

	if (scheme_css->scheme == NULL)
	{
		return;
	}

	context = gtk_widget_get_style_context (widget);
	gtk_style_context_add_provider (context,
					GTK_STYLE_PROVIDER (scheme_css->main_provider),
					GTK_SOURCE_STYLE_PROVIDER_PRIORITY);

	G_GNUC_BEGIN_IGNORE_DEPRECATIONS;
	/* See https://bugzilla.gnome.org/show_bug.cgi?id=708583 */
	gtk_style_context_invalidate (context);
	G_GNUC_END_IGNORE_DEPRECATIONS;

	/* The GtkCssProvider for the cursors needs that the first provider is
	 * applied, to get the background color.
	 */
	if (scheme_css->cursors_provider == NULL)
	{
		scheme_css->cursors_provider = create_cursors_provider (scheme_css->scheme, widget);
	}

	if (scheme_css->cursors_provider != NULL)
	{
		gtk_style_context_add_provider (context,
						GTK_STYLE_PROVIDER (scheme_css->cursors_provider),
						GTK_SOURCE_STYLE_PROVIDER_PRIORITY);

		G_GNUC_BEGIN_IGNORE_DEPRECATIONS;
		gtk_style_context_invalidate (context);
		G_GNUC_END_IGNORE_DEPRECATIONS;
	}
}

/* Removes the #GtkCssProvider's (that are part of @scheme_css) from @widget.
 * @widget is typically a #GtkSourceView.
 */
void
_gtk_source_style_scheme_css_unapply (GtkSourceStyleSchemeCss *scheme_css,
				      GtkWidget               *widget)
{
	GtkStyleContext *context;

	g_return_if_fail (scheme_css != NULL);
	g_return_if_fail (GTK_IS_WIDGET (widget));

	context = gtk_widget_get_style_context (widget);
	gtk_style_context_remove_provider (context,
					   GTK_STYLE_PROVIDER (scheme_css->main_provider));

	if (scheme_css->cursors_provider != NULL)
	{
		gtk_style_context_remove_provider (context,
						   GTK_STYLE_PROVIDER (scheme_css->cursors_provider));
	}

	G_GNUC_BEGIN_IGNORE_DEPRECATIONS;
	/* See https://bugzilla.gnome.org/show_bug.cgi?id=708583 */
	gtk_style_context_invalidate (context);
	G_GNUC_END_IGNORE_DEPRECATIONS;
}

static gchar *
get_foreground_color_css_declaration (GtkSourceStyle *style)
{
	GtkSourceStyleData *style_data;
	gchar *ret = NULL;

	if (style == NULL)
	{
		return NULL;
	}

	style_data = gtk_source_style_get_data (style);

	if (style_data->use_foreground_color)
	{
		gchar *fg_color_str = gdk_rgba_to_string (&style_data->foreground_color);
		ret = g_strdup_printf ("\tcolor: %s;\n", fg_color_str);
		g_free (fg_color_str);
	}

	g_free (style_data);
	return ret;
}

static gchar *
get_background_color_css_declaration (GtkSourceStyle *style)
{
	GtkSourceStyleData *style_data;
	gchar *ret = NULL;

	if (style == NULL)
	{
		return NULL;
	}

	style_data = gtk_source_style_get_data (style);

	if (style_data->use_background_color)
	{
		gchar *bg_color_str = gdk_rgba_to_string (&style_data->background_color);
		ret = g_strdup_printf ("\tbackground-color: %s;\n", bg_color_str);
		g_free (bg_color_str);
	}

	g_free (style_data);
	return ret;
}

static void
append_css_style (GString        *string,
                  GtkSourceStyle *style,
                  const gchar    *selector)
{
	gchar *fg_decl = get_foreground_color_css_declaration (style);
	gchar *bg_decl = get_background_color_css_declaration (style);

	if (fg_decl == NULL && bg_decl == NULL)
	{
		return;
	}

	g_string_append_printf (string,
				"%s {\n"
				"%s"
				"%s"
				"}\n",
				selector,
				fg_decl != NULL ? fg_decl : "",
				bg_decl != NULL ? bg_decl : "");

	g_free (fg_decl);
	g_free (bg_decl);
}

void
_gtk_source_style_scheme_css_load (GtkSourceStyleSchemeCss *scheme_css)
{
	GString *final_style;
	GtkSourceStyle *style;
	GtkSourceStyle *style2;

	g_return_if_fail (scheme_css != NULL);

	if (scheme_css->scheme == NULL)
	{
		return;
	}

	final_style = g_string_new ("");

	style = gtk_source_style_scheme_get_style (scheme_css->scheme, "text");
	append_css_style (final_style, style, "textview text");

	style = gtk_source_style_scheme_get_style (scheme_css->scheme, "selection");
	append_css_style (final_style, style, "textview:focus text selection");

	style2 = gtk_source_style_scheme_get_style (scheme_css->scheme, "selection-unfocused");
	append_css_style (final_style,
			  style2 != NULL ? style2 : style,
			  "textview text selection");

	/* For now we use "line-numbers" colors for all the gutters. */
	style = gtk_source_style_scheme_get_style (scheme_css->scheme, "line-numbers");
	if (style != NULL)
	{
		append_css_style (final_style, style, "textview border");

		/* Needed for GtkSourceGutter. In the ::draw callback,
		 * gtk_style_context_add_class() is called to add e.g. the
		 * "left" class. Because as of GTK+ 3.20 we cannot do the same
		 * to add the "border" subnode.
		 */
		append_css_style (final_style, style, "textview .left");
		append_css_style (final_style, style, "textview .right");
		append_css_style (final_style, style, "textview .top");
		append_css_style (final_style, style, "textview .bottom");

		/* For the corners if the top or bottom gutter is also
		 * displayed.
		 * FIXME: this shouldn't be necessary, GTK+ should apply the
		 * border style to the corners too, see:
		 * https://bugzilla.gnome.org/show_bug.cgi?id=764239
		 */
		append_css_style (final_style, style, "textview");
	}

	style = gtk_source_style_scheme_get_style (scheme_css->scheme, "current-line-number");
	if (style != NULL)
	{
		append_css_style (final_style, style, ".current-line-number");
	}

	if (*final_style->str != '\0')
	{
		GError *error = NULL;

		gtk_css_provider_load_from_data (scheme_css->main_provider,
						 final_style->str,
						 final_style->len,
						 &error);

		if (error != NULL)
		{
			g_warning ("%s", error->message);
			g_clear_error (&error);
		}
	}

	g_string_free (final_style, TRUE);
}
