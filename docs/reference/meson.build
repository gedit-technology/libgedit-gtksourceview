subdir('xml')

FS.copyfile(
  'libgedit-gtksourceview-sections.txt',
  'libgedit-gtksourceview-@0@-sections.txt'.format(LIBGEDIT_GTKSOURCEVIEW_API_VERSION)
)

gtkdoc_module_name = 'libgedit-gtksourceview-@0@'.format(LIBGEDIT_GTKSOURCEVIEW_API_VERSION)
html_dir = get_option('prefix') / GNOME.gtkdoc_html_dir(gtkdoc_module_name)

GNOME.gtkdoc(
  gtkdoc_module_name,
  main_xml: 'libgedit-gtksourceview-docs.xml',
  src_dir: include_directories('../../gtksourceview/'),
  dependencies: LIBGEDIT_GTKSOURCEVIEW_LIB_DEP,
  scan_args: ['--rebuild-types'],
  gobject_typesfile: 'libgedit-gtksourceview-@0@.types'.format(LIBGEDIT_GTKSOURCEVIEW_API_VERSION),
  fixxref_args: [
    '--html-dir=@0@'.format(html_dir),
  ],
  content_files: [
    'intro.xml',
    'lang-reference.xml',
    'lang-tutorial.xml',
    'porting-guides.xml',
    'style-reference.xml',
  ],
  ignore_headers: [
    LIBGEDIT_GTKSOURCEVIEW_PRIVATE_HEADERS,
    WORDS_COMPLETION_PROVIDER_PRIVATE_HEADERS,
  ],
  install: true
)
