test_completion_sources = ['test-completion.c']
test_completion_sources += GNOME.compile_resources(
  'test-completion-resources',
  'test-completion.gresource.xml',
)
executable(
  'test-completion',
  test_completion_sources,
  dependencies: LIBGEDIT_GTKSOURCEVIEW_LIB_DEP,
)

test_search_sources = ['test-search.c']
test_search_sources += GNOME.compile_resources(
  'test-search-resources',
  'test-search.gresource.xml',
)
executable(
  'test-search',
  test_search_sources,
  dependencies: LIBGEDIT_GTKSOURCEVIEW_LIB_DEP,
  c_args: '-DTOP_SRCDIR="@0@"'.format(meson.project_source_root()),
)

executable(
  'test-search-performances',
  'test-search-performances.c',
  dependencies: LIBGEDIT_GTKSOURCEVIEW_LIB_DEP,
)

executable(
  'test-space-drawing',
  'test-space-drawing.c',
  dependencies: LIBGEDIT_GTKSOURCEVIEW_LIB_DEP,
)

executable(
  'test-undo-manager-performances',
  'test-undo-manager-performances.c',
  dependencies: LIBGEDIT_GTKSOURCEVIEW_LIB_DEP,
)

test_widget_sources = ['test-widget.c']
test_widget_sources += GNOME.compile_resources(
  'test-widget-resources',
  'test-widget.gresource.xml',
)
executable(
  'test-widget',
  test_widget_sources,
  dependencies: LIBGEDIT_GTKSOURCEVIEW_LIB_DEP,
  c_args: '-DTOP_SRCDIR="@0@"'.format(meson.project_source_root()),
)
