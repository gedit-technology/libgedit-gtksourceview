# Convention:
# - Local variables in lower_case.
# - Global variables in UPPER_CASE.
# See: https://github.com/mesonbuild/meson/issues/2607

project(
  'libgedit-gtksourceview', 'c',
  meson_version: '>= 0.64',
  version: '299.2.2', # Do not forget to update the SOVERSION if needed.
  default_options: ['warning_level=2']
)

SOVERSION = 1

GNOME = import('gnome')
PKG_CONFIG = import('pkgconfig')
I18N = import('i18n')
FS = import('fs')

# API version, used for parallel installability.
LIBGEDIT_GTKSOURCEVIEW_API_VERSION = '300'

LIBGEDIT_GTKSOURCEVIEW_PUBLIC_DEPS = [
  dependency('gio-2.0', version: '>= 2.74'),
  dependency('gtk+-3.0', version: '>= 3.20'),
]
libgedit_gtksourceview_private_deps = [
  dependency('libxml-2.0'),
]
LIBGEDIT_GTKSOURCEVIEW_ALL_DEPS = [
  LIBGEDIT_GTKSOURCEVIEW_PUBLIC_DEPS,
  libgedit_gtksourceview_private_deps,
]

# config.h

config_data = configuration_data()
GETTEXT_PACKAGE_NAME = 'libgedit-gtksourceview-' + LIBGEDIT_GTKSOURCEVIEW_API_VERSION
config_data.set_quoted('GETTEXT_PACKAGE', GETTEXT_PACKAGE_NAME)
config_data.set_quoted('DATADIR', get_option('prefix') / get_option('datadir'))
config_data.set_quoted('GSV_API_VERSION', LIBGEDIT_GTKSOURCEVIEW_API_VERSION)

c_compiler = meson.get_compiler('c')
if c_compiler.has_header('unistd.h')
  config_data.set('HAVE_UNISTD_H', true)
endif

# TODO: for macOS support.
# the only missing thing for the config.h.
#if false
#  config_data.set('OS_OSX', true)
#endif

configure_file(
  output: 'config.h',
  configuration: config_data
)

# Misc

ROOT_INCLUDE_DIR = include_directories('.')

add_project_arguments(
  '-DG_LOG_DOMAIN="@0@"'.format(meson.project_name()),
  language: 'c'
)

#####
# CFLAGS
# Some flags are missing when using only the builtin warning_level meson option,
# even at the maximum level.
# The following warning_cflags suppose that warning_level=2.

c_compiler = meson.get_compiler('c')
warning_cflags = []

if c_compiler.get_id() == 'msvc'
  # Use GLib's msvc_recommended_pragmas.h to filter out
  # the warnings we don't really need to worry about,
  # but do make the ones we want to be wary stand out
  warning_cflags += [
    '-FImsvc_recommended_pragmas.h',
  ]
else
  # Try to mimic the AX_COMPILER_FLAGS Autotools macro.
  warning_cflags += [
    '-fno-strict-aliasing',
    '-Wundef',
    '-Wnested-externs',
    '-Wwrite-strings',
    '-Wpointer-arith',
    '-Wmissing-declarations',
    '-Wmissing-prototypes',
    '-Wstrict-prototypes',
    '-Wredundant-decls',
    '-Wno-unused-parameter',
    '-Wno-missing-field-initializers',
    '-Wdeclaration-after-statement',
    '-Wformat=2',
    '-Wold-style-definition',
    '-Wcast-align',
    '-Wformat-nonliteral',
    '-Wformat-security',
    '-Wsign-compare',
    '-Wstrict-aliasing',
    '-Wshadow',
    '-Winline',
    '-Wpacked',
    '-Wmissing-format-attribute',
    '-Wmissing-noreturn',
    '-Winit-self',
    '-Wredundant-decls',
    '-Wmissing-include-dirs',
    '-Wunused-but-set-variable',
    '-Warray-bounds',
    '-Wimplicit-function-declaration',
    '-Wreturn-type',
    '-Wswitch-enum',
    '-Wswitch-default',
    '-Wduplicated-cond',
    '-Wduplicated-branches',
    '-Wlogical-op',
    '-Wrestrict',
    '-Wnull-dereference',
    '-Wjump-misses-init',
    '-Wdouble-promotion',

    # Deprecated symbols are currently in use, but it's not urgent to port the
    # code, so silence the warnings.
    '-Wno-deprecated-declarations',
  ]
endif

supported_warning_cflags = c_compiler.get_supported_arguments(warning_cflags)
add_project_arguments(supported_warning_cflags, language: 'c')
##### end CFLAGS

subdir('data')
subdir('po')
subdir('gtksourceview')
subdir('tests')
subdir('tools/list-langs')

if get_option('gtk_doc')
  subdir('docs/reference')
endif

summary('API version', LIBGEDIT_GTKSOURCEVIEW_API_VERSION)
summary('Prefix', get_option('prefix'))
summary('GObject Introspection', get_option('gobject_introspection'))
summary('API documentation', get_option('gtk_doc'))
