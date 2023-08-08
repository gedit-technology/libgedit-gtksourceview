libgedit_gtksourceview_public_headers = [
  'gtksource.h',
  'gtksourceautocleanups.h',
  'gtksourcebuffer.h',
  'gtksourcecompletion.h',
  'gtksourcecompletioncontext.h',
  'gtksourcecompletioninfo.h',
  'gtksourcecompletionitem.h',
  'gtksourcecompletionproposal.h',
  'gtksourcecompletionprovider.h',
  'gtksourceencoding.h',
  'gtksourcefile.h',
  'gtksourcefileloader.h',
  'gtksourcefilesaver.h',
  'gtksourcegutter.h',
  'gtksourcegutterrenderer.h',
  'gtksourcegutterrendererpixbuf.h',
  'gtksourcegutterrenderertext.h',
  'gtksourceinit.h',
  'gtksourcelanguage.h',
  'gtksourcelanguagemanager.h',
  'gtksourcemark.h',
  'gtksourcemarkattributes.h',
  'gtksourceprintcompositor.h',
  'gtksourceregion.h',
  'gtksourcesearchcontext.h',
  'gtksourcesearchsettings.h',
  'gtksourcespacedrawer.h',
  'gtksourcestyle.h',
  'gtksourcestylescheme.h',
  'gtksourcestyleschemechooser.h',
  'gtksourcestyleschememanager.h',
  'gtksourcetag.h',
  'gtksourcetypes.h',
  'gtksourceundomanager.h',
  'gtksourceutils.h',
  'gtksourceview.h',
]

libgedit_gtksourceview_public_c_files = [
  'gtksourcebuffer.c',
  'gtksourcecompletion.c',
  'gtksourcecompletioncontext.c',
  'gtksourcecompletioninfo.c',
  'gtksourcecompletionitem.c',
  'gtksourcecompletionproposal.c',
  'gtksourcecompletionprovider.c',
  'gtksourceencoding.c',
  'gtksourcefile.c',
  'gtksourcefileloader.c',
  'gtksourcefilesaver.c',
  'gtksourcegutter.c',
  'gtksourcegutterrenderer.c',
  'gtksourcegutterrendererpixbuf.c',
  'gtksourcegutterrenderertext.c',
  'gtksourceinit.c',
  'gtksourcelanguage.c',
  'gtksourcelanguagemanager.c',
  'gtksourcemark.c',
  'gtksourcemarkattributes.c',
  'gtksourceprintcompositor.c',
  'gtksourceregion.c',
  'gtksourcesearchcontext.c',
  'gtksourcesearchsettings.c',
  'gtksourcespacedrawer.c',
  'gtksourcestyle.c',
  'gtksourcestylescheme.c',
  'gtksourcestyleschemechooser.c',
  'gtksourcestyleschememanager.c',
  'gtksourcetag.c',
  'gtksourceundomanager.c',
  'gtksourceutils.c',
  'gtksourceview.c',
]

LIBGEDIT_GTKSOURCEVIEW_PRIVATE_HEADERS = [
  'gtksourcebuffer-private.h',
  'gtksourcebufferinputstream.h',
  'gtksourcebufferinternal.h',
  'gtksourcebufferoutputstream.h',
  'gtksourcecompletioncontainer.h',
  'gtksourcecompletionmodel.h',
  'gtksourcecompletion-private.h',
  'gtksourcecontextengine.h',
  'gtksourceencoding-private.h',
  'gtksourceengine.h',
  'gtksourcegutter-private.h',
  'gtksourcegutterrendererlines.h',
  'gtksourcegutterrenderermarks.h',
  'gtksourcegutterrenderer-private.h',
  'gtksourceiter.h',
  'gtksourcelanguage-private.h',
  'gtksourcemarkssequence.h',
  'gtksourcepixbufhelper.h',
  'gtksourceregex.h',
  'gtksourcespacedrawer-private.h',
  'gtksourcestylescheme-private.h',
  'gtksourcestyleschemecss.h',
  'gtksourcestyleschemeparser.h',
  'gtksourcetypes-private.h',
  'gtksourceundomanagerdefault.h',
  'gtksourceutils-private.h',
]

libgedit_gtksourceview_private_c_files = [
  'gtksourcebufferinputstream.c',
  'gtksourcebufferinternal.c',
  'gtksourcebufferoutputstream.c',
  'gtksourcecompletioncontainer.c',
  'gtksourcecompletionmodel.c',
  'gtksourcecontextengine.c',
  'gtksourceengine.c',
  'gtksourcegutterrendererlines.c',
  'gtksourcegutterrenderermarks.c',
  'gtksourceiter.c',
  'gtksourcelanguage-parser-2.c',
  'gtksourcemarkssequence.c',
  'gtksourcepixbufhelper.c',
  'gtksourceregex.c',
  'gtksourcestyleschemecss.c',
  'gtksourcestyleschemeparser.c',
  'gtksourceundomanagerdefault.c',
]

headers_install_dir = get_option('includedir') / 'libgedit-gtksourceview-' + LIBGEDIT_GTKSOURCEVIEW_API_VERSION / 'gtksourceview'
install_headers(
  libgedit_gtksourceview_public_headers,
  install_dir: headers_install_dir
)

LIBGEDIT_GTKSOURCEVIEW_ENUM_TYPES = GNOME.mkenums(
  'gtksource-enumtypes',
  c_template: 'gtksource-enumtypes.c.in',
  h_template: 'gtksource-enumtypes.h.in',
  identifier_prefix: 'GtkSource',
  install_header: true,
  install_dir: headers_install_dir,
  sources: libgedit_gtksourceview_public_headers,
  symbol_prefix: 'gtk_source',
)

libgedit_gtksourceview_resources = GNOME.compile_resources(
  'gtksourceview-resources',
  'gtksourceview.gresource.xml',
)

libgedit_gtksourceview_static_lib = static_library(
  'libgedit-gtksourceview-static',
  [libgedit_gtksourceview_public_headers,
   libgedit_gtksourceview_public_c_files,
   LIBGEDIT_GTKSOURCEVIEW_PRIVATE_HEADERS,
   libgedit_gtksourceview_private_c_files,
   LIBGEDIT_GTKSOURCEVIEW_ENUM_TYPES,
   libgedit_gtksourceview_resources],
  pic: true, # this static lib is potentially linked in a shared library.
  include_directories: ROOT_INCLUDE_DIR,
  dependencies: LIBGEDIT_GTKSOURCEVIEW_ALL_DEPS,
  c_args: [
    '-DGTK_SOURCE_COMPILATION',
    '-DHAVE_CONFIG_H',
  ]
)

# The words completion provider requires that the enumtypes are already
# generated (which is done above).
subdir('completion-providers/words')

# For unit tests, to be able to test private functions.
LIBGEDIT_GTKSOURCEVIEW_STATIC_DEP = declare_dependency(
  include_directories: ROOT_INCLUDE_DIR,
  link_with: [
    libgedit_gtksourceview_static_lib,
    WORDS_COMPLETION_PROVIDER_STATIC_LIB,
  ],
  sources: LIBGEDIT_GTKSOURCEVIEW_ENUM_TYPES[1],
  dependencies: LIBGEDIT_GTKSOURCEVIEW_ALL_DEPS
)

symbol_map = 'symbol.map'
libgedit_gtksourceview_link_args = c_compiler.get_supported_link_arguments([
  '-Wl,--version-script,@0@/@1@'.format(meson.current_source_dir(), symbol_map),
])

libgedit_gtksourceview_lib = library(
  'gedit-gtksourceview-@0@'.format(LIBGEDIT_GTKSOURCEVIEW_API_VERSION),
  dependencies: LIBGEDIT_GTKSOURCEVIEW_ALL_DEPS,
  link_args: libgedit_gtksourceview_link_args,
  link_depends: symbol_map,
  # link_whole is not supported with MSVC, so we use extract_all_objects().
  objects: [
    libgedit_gtksourceview_static_lib.extract_all_objects(recursive: false),
    WORDS_COMPLETION_PROVIDER_STATIC_LIB.extract_all_objects(recursive: false),
  ],
  soversion: 0,
  install: true
)

LIBGEDIT_GTKSOURCEVIEW_LIB_DEP = declare_dependency(
  include_directories: ROOT_INCLUDE_DIR,
  link_with: libgedit_gtksourceview_lib,
  sources: LIBGEDIT_GTKSOURCEVIEW_ENUM_TYPES[1],
  dependencies: LIBGEDIT_GTKSOURCEVIEW_ALL_DEPS
)

PKG_CONFIG.generate(libgedit_gtksourceview_lib,
  filebase: 'libgedit-gtksourceview-@0@'.format(LIBGEDIT_GTKSOURCEVIEW_API_VERSION),
  name: 'libgedit-gtksourceview',
  description: 'Gedit Technology - Source code editing widget',
  subdirs: 'libgedit-gtksourceview-@0@'.format(LIBGEDIT_GTKSOURCEVIEW_API_VERSION),
  libraries: LIBGEDIT_GTKSOURCEVIEW_PUBLIC_DEPS,
)

if get_option('gobject_introspection')
  GNOME.generate_gir(
    libgedit_gtksourceview_lib,
    export_packages: 'libgedit-gtksourceview-@0@'.format(LIBGEDIT_GTKSOURCEVIEW_API_VERSION),
    header: 'gtksourceview/gtksource.h',
    identifier_prefix: 'GtkSource',
    include_directories: ROOT_INCLUDE_DIR,
    includes: ['Gtk-3.0', 'Gdk-3.0'],
    install: true,
    namespace: 'GtkSource',
    nsversion: LIBGEDIT_GTKSOURCEVIEW_API_VERSION,
    sources: [
      libgedit_gtksourceview_public_headers,
      libgedit_gtksourceview_public_c_files,
      LIBGEDIT_GTKSOURCEVIEW_ENUM_TYPES
    ],
    # Support for deps being built as subprojects:
    dependencies: LIBGEDIT_GTKSOURCEVIEW_ALL_DEPS,
  )
endif
