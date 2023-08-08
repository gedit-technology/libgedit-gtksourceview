/* SPDX-FileCopyrightText: 2023 - Sébastien Wilmet
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <gtksourceview/gtksource.h>

static void
test_basics (void)
{
	GtkSourceStyle *style;
	GtkTextTag *tag;
	PangoStyle pango_style = PANGO_STYLE_NORMAL;
	gboolean pango_style_set = FALSE;

	style = gtk_source_style_new ();
	g_assert_false (style->use_italic);

	style->italic = TRUE;
	style->use_italic = TRUE;

	tag = gtk_text_tag_new (NULL);
	gtk_source_style_apply (style, tag);

	g_object_get (tag,
		      "style", &pango_style,
		      "style-set", &pango_style_set,
		      NULL);
	g_assert_true (pango_style == PANGO_STYLE_ITALIC);
	g_assert_true (pango_style_set);

	gtk_source_style_unref (style);
	g_object_unref (tag);
}

int
main (int    argc,
      char **argv)
{
	gint ret;

	gtk_test_init (&argc, &argv);
	gtk_source_init ();

	g_test_add_func ("/Style/basics", test_basics);

	ret = g_test_run ();
	gtk_source_finalize ();

	return ret;
}
