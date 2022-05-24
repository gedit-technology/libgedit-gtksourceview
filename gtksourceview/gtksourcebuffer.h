/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *
 * This file is part of GtkSourceView
 *
 * Copyright (C) 1999-2002 - Mikael Hermansson <tyan@linux.se>,
 *                           Chris Phelps <chicane@reninet.com> and
 *                           Jeroen Zwartepoorte <jeroen@xs4all.nl>
 * Copyright (C) 2003 - Paolo Maggi, Gustavo Giráldez
 * Copyright (C) 2014 - Sébastien Wilmet <swilmet@gnome.org>
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

#ifndef GTK_SOURCE_BUFFER_H
#define GTK_SOURCE_BUFFER_H

#if !defined (GTK_SOURCE_H_INSIDE) && !defined (GTK_SOURCE_COMPILATION)
#error "Only <gtksourceview/gtksource.h> can be included directly."
#endif

#include <gtk/gtk.h>
#include <gtksourceview/gtksourcetypes.h>

G_BEGIN_DECLS

#define GTK_SOURCE_TYPE_BUFFER            (gtk_source_buffer_get_type ())
#define GTK_SOURCE_BUFFER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_SOURCE_TYPE_BUFFER, GtkSourceBuffer))
#define GTK_SOURCE_BUFFER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_SOURCE_TYPE_BUFFER, GtkSourceBufferClass))
#define GTK_SOURCE_IS_BUFFER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_SOURCE_TYPE_BUFFER))
#define GTK_SOURCE_IS_BUFFER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_SOURCE_TYPE_BUFFER))
#define GTK_SOURCE_BUFFER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_SOURCE_TYPE_BUFFER, GtkSourceBufferClass))

typedef struct _GtkSourceBufferClass		GtkSourceBufferClass;
typedef struct _GtkSourceBufferPrivate		GtkSourceBufferPrivate;

/**
 * GtkSourceBracketMatchType:
 * @GTK_SOURCE_BRACKET_MATCH_NONE: there is no bracket to match.
 * @GTK_SOURCE_BRACKET_MATCH_OUT_OF_RANGE: matching a bracket
 *  failed because the maximum range was reached.
 * @GTK_SOURCE_BRACKET_MATCH_NOT_FOUND: a matching bracket was not found.
 * @GTK_SOURCE_BRACKET_MATCH_FOUND: a matching bracket was found.
 */
typedef enum _GtkSourceBracketMatchType
{
	GTK_SOURCE_BRACKET_MATCH_NONE,
	GTK_SOURCE_BRACKET_MATCH_OUT_OF_RANGE,
	GTK_SOURCE_BRACKET_MATCH_NOT_FOUND,
	GTK_SOURCE_BRACKET_MATCH_FOUND
} GtkSourceBracketMatchType;

/**
 * GtkSourceChangeCaseType:
 * @GTK_SOURCE_CHANGE_CASE_LOWER: change case to lowercase.
 * @GTK_SOURCE_CHANGE_CASE_UPPER: change case to uppercase.
 * @GTK_SOURCE_CHANGE_CASE_TOGGLE: toggle case of each character.
 * @GTK_SOURCE_CHANGE_CASE_TITLE: capitalize each word.
 *
 * Since: 3.12
 */
typedef enum _GtkSourceChangeCaseType
{
	GTK_SOURCE_CHANGE_CASE_LOWER,
	GTK_SOURCE_CHANGE_CASE_UPPER,
	GTK_SOURCE_CHANGE_CASE_TOGGLE,
	GTK_SOURCE_CHANGE_CASE_TITLE
} GtkSourceChangeCaseType;

/**
 * GtkSourceSortFlags:
 * @GTK_SOURCE_SORT_FLAGS_NONE: no flags specified
 * @GTK_SOURCE_SORT_FLAGS_CASE_SENSITIVE: case sensitive sort
 * @GTK_SOURCE_SORT_FLAGS_REVERSE_ORDER: sort in reverse order
 * @GTK_SOURCE_SORT_FLAGS_REMOVE_DUPLICATES: remove duplicates
 *
 * Since: 3.18
 */
typedef enum _GtkSourceSortFlags
{
	GTK_SOURCE_SORT_FLAGS_NONE              = 0,
	GTK_SOURCE_SORT_FLAGS_CASE_SENSITIVE    = 1 << 0,
	GTK_SOURCE_SORT_FLAGS_REVERSE_ORDER     = 1 << 1,
	GTK_SOURCE_SORT_FLAGS_REMOVE_DUPLICATES = 1 << 2,
} GtkSourceSortFlags;

struct _GtkSourceBuffer
{
	GtkTextBuffer parent_instance;

	GtkSourceBufferPrivate *priv;
};

struct _GtkSourceBufferClass
{
	GtkTextBufferClass parent_class;

	/* Signals */
	void (*undo) (GtkSourceBuffer *buffer);
	void (*redo) (GtkSourceBuffer *buffer);

	void (*bracket_matched) (GtkSourceBuffer           *buffer,
				 GtkTextIter               *iter,
				 GtkSourceBracketMatchType  state);

	/* Padding for future expansion */
	gpointer padding[20];
};

G_MODULE_EXPORT
GType			 gtk_source_buffer_get_type				(void) G_GNUC_CONST;

G_MODULE_EXPORT
GtkSourceBuffer	 	*gtk_source_buffer_new					(GtkTextTagTable        *table);

G_MODULE_EXPORT
GtkSourceBuffer 	*gtk_source_buffer_new_with_language			(GtkSourceLanguage      *language);

G_MODULE_EXPORT
gboolean		 gtk_source_buffer_get_highlight_syntax			(GtkSourceBuffer        *buffer);

G_MODULE_EXPORT
void			 gtk_source_buffer_set_highlight_syntax			(GtkSourceBuffer        *buffer,
										 gboolean                highlight);

G_MODULE_EXPORT
gboolean		 gtk_source_buffer_get_highlight_matching_brackets	(GtkSourceBuffer        *buffer);

G_MODULE_EXPORT
void			 gtk_source_buffer_set_highlight_matching_brackets	(GtkSourceBuffer        *buffer,
										 gboolean                highlight);

G_MODULE_EXPORT
gint			 gtk_source_buffer_get_max_undo_levels			(GtkSourceBuffer        *buffer);

G_MODULE_EXPORT
void			 gtk_source_buffer_set_max_undo_levels			(GtkSourceBuffer        *buffer,
										 gint                    max_undo_levels);

G_MODULE_EXPORT
GtkSourceLanguage 	*gtk_source_buffer_get_language				(GtkSourceBuffer        *buffer);

G_MODULE_EXPORT
void			 gtk_source_buffer_set_language				(GtkSourceBuffer        *buffer,
										 GtkSourceLanguage      *language);

G_MODULE_EXPORT
gboolean		 gtk_source_buffer_can_undo				(GtkSourceBuffer        *buffer);

G_MODULE_EXPORT
gboolean		 gtk_source_buffer_can_redo				(GtkSourceBuffer        *buffer);

G_MODULE_EXPORT
GtkSourceStyleScheme    *gtk_source_buffer_get_style_scheme			(GtkSourceBuffer        *buffer);

G_MODULE_EXPORT
void			 gtk_source_buffer_set_style_scheme			(GtkSourceBuffer        *buffer,
										 GtkSourceStyleScheme   *scheme);

G_MODULE_EXPORT
void			 gtk_source_buffer_ensure_highlight			(GtkSourceBuffer        *buffer,
										 const GtkTextIter      *start,
										 const GtkTextIter      *end);

G_MODULE_EXPORT
void			 gtk_source_buffer_undo					(GtkSourceBuffer        *buffer);

G_MODULE_EXPORT
void			 gtk_source_buffer_redo					(GtkSourceBuffer        *buffer);

G_MODULE_EXPORT
void			 gtk_source_buffer_begin_not_undoable_action		(GtkSourceBuffer	*buffer);

G_MODULE_EXPORT
void			 gtk_source_buffer_end_not_undoable_action		(GtkSourceBuffer	*buffer);

G_MODULE_EXPORT
GtkSourceMark		*gtk_source_buffer_create_source_mark			(GtkSourceBuffer        *buffer,
										 const gchar            *name,
										 const gchar            *category,
										 const GtkTextIter      *where);

G_MODULE_EXPORT
gboolean		 gtk_source_buffer_forward_iter_to_source_mark		(GtkSourceBuffer        *buffer,
										 GtkTextIter            *iter,
										 const gchar            *category);

G_MODULE_EXPORT
gboolean		 gtk_source_buffer_backward_iter_to_source_mark		(GtkSourceBuffer        *buffer,
										 GtkTextIter            *iter,
										 const gchar            *category);

G_MODULE_EXPORT
GSList			*gtk_source_buffer_get_source_marks_at_iter		(GtkSourceBuffer        *buffer,
										 GtkTextIter            *iter,
										 const gchar            *category);

G_MODULE_EXPORT
GSList			*gtk_source_buffer_get_source_marks_at_line		(GtkSourceBuffer        *buffer,
										 gint 			 line,
										 const gchar		*category);

G_MODULE_EXPORT
void			 gtk_source_buffer_remove_source_marks			(GtkSourceBuffer        *buffer,
										 const GtkTextIter      *start,
										 const GtkTextIter      *end,
										 const gchar            *category);

G_MODULE_EXPORT
gboolean		 gtk_source_buffer_iter_has_context_class		(GtkSourceBuffer	*buffer,
										 const GtkTextIter	*iter,
										 const gchar            *context_class);

G_MODULE_EXPORT
gchar		       **gtk_source_buffer_get_context_classes_at_iter		(GtkSourceBuffer	*buffer,
										 const GtkTextIter	*iter);

G_MODULE_EXPORT
gboolean		 gtk_source_buffer_iter_forward_to_context_class_toggle	(GtkSourceBuffer	*buffer,
										 GtkTextIter		*iter,
										 const gchar		*context_class);

G_MODULE_EXPORT
gboolean		 gtk_source_buffer_iter_backward_to_context_class_toggle
										(GtkSourceBuffer	*buffer,
										 GtkTextIter		*iter,
										 const gchar		*context_class);

G_MODULE_EXPORT
void			 gtk_source_buffer_change_case				(GtkSourceBuffer        *buffer,
										 GtkSourceChangeCaseType case_type,
										 GtkTextIter            *start,
										 GtkTextIter            *end);

G_MODULE_EXPORT
void			 gtk_source_buffer_join_lines				(GtkSourceBuffer        *buffer,
										 GtkTextIter            *start,
										 GtkTextIter            *end);

G_MODULE_EXPORT
void			 gtk_source_buffer_sort_lines				(GtkSourceBuffer        *buffer,
										 GtkTextIter            *start,
										 GtkTextIter            *end,
										 GtkSourceSortFlags     flags,
										 gint                   column);

G_MODULE_EXPORT
GtkSourceUndoManager	*gtk_source_buffer_get_undo_manager			(GtkSourceBuffer	*buffer);

G_MODULE_EXPORT
void			 gtk_source_buffer_set_undo_manager			(GtkSourceBuffer	*buffer,
										 GtkSourceUndoManager	*manager);

G_MODULE_EXPORT
void			 gtk_source_buffer_set_implicit_trailing_newline	(GtkSourceBuffer        *buffer,
										 gboolean                implicit_trailing_newline);

G_MODULE_EXPORT
gboolean		 gtk_source_buffer_get_implicit_trailing_newline	(GtkSourceBuffer        *buffer);

G_MODULE_EXPORT
GtkTextTag		*gtk_source_buffer_create_source_tag			(GtkSourceBuffer        *buffer,
										 const gchar            *tag_name,
										 const gchar            *first_property_name,
										 ...);

G_END_DECLS

#endif /* GTK_SOURCE_BUFFER_H */
