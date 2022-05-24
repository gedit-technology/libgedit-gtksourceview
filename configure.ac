AC_PREREQ(2.64)

m4_define(gtksourceview_major_version, 299)
m4_define(gtksourceview_minor_version, 0)
m4_define(gtksourceview_micro_version, 0)
m4_define(gtksourceview_version, gtksourceview_major_version.gtksourceview_minor_version.gtksourceview_micro_version)

AC_INIT([libgedit-gtksourceview],
	[gtksourceview_version],
	[https://github.com/gedit-technology/libgedit-gtksourceview/issues],
	[libgedit-gtksourceview],
	[https://gedit-technology.net/])

AX_IS_RELEASE([git-directory])

GTK_SOURCE_MAJOR_VERSION=gtksourceview_major_version
GTK_SOURCE_MINOR_VERSION=gtksourceview_minor_version
GTK_SOURCE_MICRO_VERSION=gtksourceview_micro_version
AC_SUBST(GTK_SOURCE_MAJOR_VERSION)
AC_SUBST(GTK_SOURCE_MINOR_VERSION)
AC_SUBST(GTK_SOURCE_MICRO_VERSION)

# Libtool versioning
#
# For a new release, apply the following algorithm step by step:
# 1. If the library source code has changed at all since the last
#    update, then increment REVISION.
# 2. If any exported functions or data have been added, removed, or
#    changed since the last update, increment CURRENT and set REVISION
#    to 0.
# 3. If any exported functions or data have been added since the last
#    public release, increment AGE.
# 4. If any exported functions or data have been removed since the last
#    public release, set AGE to 0.
#
# When incrementing the API version (usually for a new major package version),
# set CURRENT, REVISION and AGE to 0 since it's like a new library.
GSV_LT_CURRENT=0
GSV_LT_REVISION=0
GSV_LT_AGE=0
GSV_LT_VERSION="$GSV_LT_CURRENT:$GSV_LT_REVISION:$GSV_LT_AGE"
AC_SUBST([GSV_LT_VERSION])

# API version, used for parallel installability.
# Not used in:
# - po/Makevars
# - the docs/reference/libgedit-gtksourceview-300-sections.txt filename
# Because as far as I've tested, it is not easily feasible to not hardcode the
# API version in those places (swilmet).
GSV_API_VERSION=300
AC_SUBST([GSV_API_VERSION])
AC_DEFINE_UNQUOTED([GSV_API_VERSION],
		   ["$GSV_API_VERSION"],
		   [Define to the libgedit-gtksourceview API version.])

# Dependencies
glib_req=2.60
gtk_req=3.20

AC_CONFIG_SRCDIR([gtksourceview/gtksourcebuffer.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# gnu strictness to generate the INSTALL file
AM_INIT_AUTOMAKE([1.13 tar-ustar dist-xz no-dist-gzip -Wno-portability gnu])
AM_MAINTAINER_MODE([enable])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Check for programs
AC_PROG_CC
AC_PROG_INSTALL

AX_REQUIRE_DEFINED([PKG_PROG_PKG_CONFIG])
AX_REQUIRE_DEFINED([PKG_INSTALLDIR])
AX_REQUIRE_DEFINED([PKG_CHECK_VAR])
PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

# Initialize libtool
LT_PREREQ([2.2.6])
LT_INIT([disable-static])

# Check for header files
AC_CHECK_HEADERS([unistd.h])

# Pull glib-mkenums & co.
PKG_CHECK_VAR([GLIB_MKENUMS], [glib-2.0], [glib_mkenums])
PKG_CHECK_VAR([GLIB_COMPILE_RESOURCES], [gio-2.0], [glib_compile_resources])
AC_PATH_PROG(ITSTOOL, itstool)

# Compile warnings. There can be deprecation warnings at any time, so disable
# -Werror by default with the [yes] argument.
AX_REQUIRE_DEFINED([AX_COMPILER_FLAGS])
AX_COMPILER_FLAGS([WARN_CFLAGS], [WARN_LDFLAGS], [yes])

# Check if the compiler supports -fvisibility=hidden
SAVED_CFLAGS="${CFLAGS}"
CFLAGS="-fvisibility=hidden"
AC_MSG_CHECKING([for -fvisibility=hidden compiler flag])
AC_TRY_COMPILE([], [return 0],
               AC_MSG_RESULT(yes)
               enable_fvisibility_hidden=yes,
               AC_MSG_RESULT(no)
               enable_fvisibility_hidden=no)
CFLAGS="${SAVED_CFLAGS}"

HIDDEN_VISIBILITY_CFLAGS=""
if test "x${enable_fvisibility_hidden}" = "xyes"
then
	HIDDEN_VISIBILITY_CFLAGS="-fvisibility=hidden"
fi
AC_SUBST([HIDDEN_VISIBILITY_CFLAGS])

# Dynamic linking dependencies
LT_LIB_M

AX_REQUIRE_DEFINED([AX_PKG_CHECK_MODULES])
AX_PKG_CHECK_MODULES([DEP],
		     [glib-2.0 >= $glib_req  gio-2.0 >= $glib_req  gtk+-3.0 >= $gtk_req],
		     [libxml-2.0])

dnl check for native osx
AC_MSG_CHECKING([for native Mac OS X])
AX_REQUIRE_DEFINED([GTK_CHECK_BACKEND])
GTK_CHECK_BACKEND([quartz], [3.0.0], [os_osx=yes], [os_osx=no])
AC_MSG_RESULT([$os_osx])
AM_CONDITIONAL([OS_OSX], [ test "$os_osx" = "yes" ])

if test "$os_osx" = "yes"; then
	AC_DEFINE([OS_OSX], [1], [Defined if os is Mac OSX])
fi

# i18n
AM_GNU_GETTEXT([external])
# FIXME: Remove AM_GNU_GETTEXT_VERSION once autoreconf supports AM_GNU_GETTEXT_REQUIRE_VERSION.
AM_GNU_GETTEXT_VERSION([0.19.6])
AM_GNU_GETTEXT_REQUIRE_VERSION([0.19.6])

AC_SUBST([GETTEXT_PACKAGE], [libgedit-gtksourceview-$GSV_API_VERSION])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Define to the gettext package name.])

# Documentation
AX_REQUIRE_DEFINED([GTK_DOC_CHECK])
GTK_DOC_CHECK([1.25], [--flavour=no-tmpl])

# Installed Tests
#
# Initially done for Gnome Continuous, but can also be useful on Windows with
# MSYS2 in case an executable needs to be installed in order to test it.
AC_ARG_ENABLE(installed_tests,
              AS_HELP_STRING([--enable-installed-tests],
                             [Install test programs (default: no)]),,
              [enable_installed_tests=no])

AM_CONDITIONAL(INSTALLED_TESTS, test "x$enable_installed_tests" = "xyes")

# Introspection
AX_REQUIRE_DEFINED([GOBJECT_INTROSPECTION_CHECK])
GOBJECT_INTROSPECTION_CHECK([1.42.0])

# Code coverage
AX_REQUIRE_DEFINED([AX_CODE_COVERAGE])
AX_CODE_COVERAGE()

# Valgrind check
AX_REQUIRE_DEFINED([AX_VALGRIND_CHECK])
AX_VALGRIND_CHECK
PKG_CHECK_VAR([GLIB_PREFIX], [glib-2.0], [prefix])

# test-widget uses this to find lang files and gtksourcebuffer.c
ABS_TOP_SRCDIR=`cd $srcdir && pwd`
AC_SUBST(ABS_TOP_SRCDIR)

# Output files
AC_CONFIG_FILES([
	libgedit-gtksourceview-${GSV_API_VERSION}.pc:libgedit-gtksourceview.pc.in
	Makefile
	data/Makefile
	data/language-specs/Makefile
	data/styles/Makefile
	docs/Makefile
	docs/reference/Makefile
	docs/reference/intro.xml
	docs/reference/lang-reference.xml
	docs/reference/libgedit-gtksourceview-docs.xml
	docs/reference/style-reference.xml
	gtksourceview/Makefile
	gtksourceview/gtksourceversion.h
	gtksourceview/completion-providers/Makefile
	gtksourceview/completion-providers/words/Makefile
	po/Makefile.in
	tests/Makefile
	testsuite/Makefile
])

AC_OUTPUT

echo "

Configuration:

	${PACKAGE_NAME} version ${PACKAGE_VERSION}

	Source code location:	${srcdir}
	Prefix:			${prefix}
	Compiler:		${CC}
	Documentation:		${enable_gtk_doc}
	GObject introspection:	${found_introspection}
	Code coverage:		${enable_code_coverage}
	Valgrind check:		${enable_valgrind}
	Installed tests:	${enable_installed_tests}
"
