/*
 * This file is part of GtkSourceView
 *
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
#include "gtksourceview/gtksourcestyleschemeparser.h"

static void
test_parse_final_color (void)
{
	GdkRGBA rgba;

	g_assert_false (_gtk_source_style_scheme_parser_parse_final_color ("", &rgba));

	g_assert_true (_gtk_source_style_scheme_parser_parse_final_color ("#000000", &rgba));
	g_assert_false (_gtk_source_style_scheme_parser_parse_final_color ("000000", &rgba));
	g_assert_false (_gtk_source_style_scheme_parser_parse_final_color ("##000000", &rgba));

	g_assert_true (_gtk_source_style_scheme_parser_parse_final_color ("#black", &rgba));
	g_assert_false (_gtk_source_style_scheme_parser_parse_final_color ("black", &rgba));

	g_assert_true (_gtk_source_style_scheme_parser_parse_final_color ("#rgb(255,0,0)", &rgba));
	g_assert_false (_gtk_source_style_scheme_parser_parse_final_color ("rgb(255,0,0)", &rgba));

	g_assert_true (_gtk_source_style_scheme_parser_parse_final_color ("#rgba(255,0,0,0.5)", &rgba));
	g_assert_false (_gtk_source_style_scheme_parser_parse_final_color ("rgba(255,0,0,0.5)", &rgba));
}

static void
test_parse_scale (void)
{
	gdouble val = 0.0;

	g_assert_false (_gtk_source_style_scheme_parser_parse_scale ("", NULL));
	g_assert_false (_gtk_source_style_scheme_parser_parse_scale ("foo", NULL));
	g_assert_false (_gtk_source_style_scheme_parser_parse_scale ("0", NULL));
	g_assert_false (_gtk_source_style_scheme_parser_parse_scale ("0.0", NULL));
	g_assert_false (_gtk_source_style_scheme_parser_parse_scale ("-1", NULL));

	g_assert_true (_gtk_source_style_scheme_parser_parse_scale ("1.2", NULL));
	g_assert_true (_gtk_source_style_scheme_parser_parse_scale ("1.2", &val));
	g_assert_cmpfloat (val, ==, 1.2);

	g_assert_true (_gtk_source_style_scheme_parser_parse_scale ("x-large", &val));
	g_assert_false (_gtk_source_style_scheme_parser_parse_scale ("X-Large", &val));
	g_assert_false (_gtk_source_style_scheme_parser_parse_scale (" x-large", &val));
}

int
main (int    argc,
      char **argv)
{
	gint ret;

	gtk_test_init (&argc, &argv);
	gtk_source_init ();

	g_test_add_func ("/StyleSchemeParser/parse_final_color", test_parse_final_color);
	g_test_add_func ("/StyleSchemeParser/parse_scale", test_parse_scale);

	ret = g_test_run ();
	gtk_source_finalize ();

	return ret;
}
