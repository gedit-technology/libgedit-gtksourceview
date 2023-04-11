SUBDIRS = completion-providers

include $(top_srcdir)/aminclude_static.am
clean-local: code-coverage-clean
distclean-local: code-coverage-dist-clean

AM_CPPFLAGS = 						\
	-DDATADIR=\""$(datadir)"\" 			\
	-DG_LOG_DOMAIN=\"libgedit-gtksourceview\"	\
	-DGTK_SOURCE_COMPILATION			\
	-I$(top_builddir)				\
	-I$(top_srcdir) -I$(srcdir) 			\
	$(WARN_CFLAGS) 					\
	$(CODE_COVERAGE_CPPFLAGS)			\
	$(GTK_MAC_CFLAGS)				\
	$(DEP_CFLAGS)

BUILT_SOURCES = 			\
	gtksource-enumtypes.c		\
	gtksource-enumtypes.h		\
	gtksourceview-resources.c

libgtksourceview_public_headers =		\
	gtksource.h				\
	gtksourceautocleanups.h			\
	gtksourcebuffer.h			\
	gtksourcecompletion.h			\
	gtksourcecompletioncontext.h		\
	gtksourcecompletioninfo.h		\
	gtksourcecompletionitem.h		\
	gtksourcecompletionproposal.h		\
	gtksourcecompletionprovider.h		\
	gtksourceencoding.h			\
	gtksourcefile.h				\
	gtksourcefileloader.h			\
	gtksourcefilesaver.h			\
	gtksourcegutter.h			\
	gtksourcegutterrenderer.h		\
	gtksourcegutterrendererpixbuf.h		\
	gtksourcegutterrenderertext.h		\
	gtksourceinit.h				\
	gtksourcelanguage.h			\
	gtksourcelanguagemanager.h		\
	gtksourcemap.h				\
	gtksourcemark.h				\
	gtksourcemarkattributes.h		\
	gtksourceprintcompositor.h		\
	gtksourceregion.h			\
	gtksourcesearchcontext.h		\
	gtksourcesearchsettings.h		\
	gtksourcespacedrawer.h			\
	gtksourcestyle.h			\
	gtksourcestylescheme.h			\
	gtksourcestyleschemechooser.h		\
	gtksourcestyleschemechooserbutton.h	\
	gtksourcestyleschemechooserwidget.h	\
	gtksourcestyleschememanager.h		\
	gtksourcetag.h				\
	gtksourcetypes.h			\
	gtksourceundomanager.h			\
	gtksourceutils.h			\
	gtksourceview.h

libgtksourceview_public_c_files =		\
	gtksourcebuffer.c 			\
	gtksourcecompletion.c			\
	gtksourcecompletioncontext.c		\
	gtksourcecompletioninfo.c		\
	gtksourcecompletionitem.c		\
	gtksourcecompletionproposal.c		\
	gtksourcecompletionprovider.c		\
	gtksourceencoding.c			\
	gtksourcefile.c				\
	gtksourcefileloader.c			\
	gtksourcefilesaver.c			\
	gtksourcegutter.c			\
	gtksourcegutterrenderer.c		\
	gtksourcegutterrendererpixbuf.c		\
	gtksourcegutterrenderertext.c		\
	gtksourceinit.c				\
	gtksourcelanguage.c 			\
	gtksourcelanguagemanager.c 		\
	gtksourcemap.c				\
	gtksourcemark.c				\
	gtksourcemarkattributes.c		\
	gtksourceprintcompositor.c		\
	gtksourceregion.c			\
	gtksourcesearchcontext.c		\
	gtksourcesearchsettings.c		\
	gtksourcespacedrawer.c			\
	gtksourcestyle.c			\
	gtksourcestylescheme.c			\
	gtksourcestyleschemechooser.c		\
	gtksourcestyleschemechooserbutton.c	\
	gtksourcestyleschemechooserwidget.c	\
	gtksourcestyleschememanager.c		\
	gtksourcetag.c				\
	gtksourceundomanager.c 			\
	gtksourceutils.c			\
	gtksourceversion.c			\
	gtksourceview.c

libgtksourceview_private_headers =		\
	gtksourcebuffer-private.h		\
	gtksourcebufferinputstream.h		\
	gtksourcebufferinternal.h		\
	gtksourcebufferoutputstream.h		\
	gtksourcecompletioncontainer.h		\
	gtksourcecompletionmodel.h		\
	gtksourcecompletion-private.h		\
	gtksourcecontextengine.h		\
	gtksourceencoding-private.h		\
	gtksourceengine.h			\
	gtksourcegutter-private.h		\
	gtksourcegutterrendererlines.h		\
	gtksourcegutterrenderermarks.h		\
	gtksourcegutterrenderer-private.h	\
	gtksourceiter.h				\
	gtksourcelanguage-private.h		\
	gtksourcemarkssequence.h		\
	gtksourcepixbufhelper.h			\
	gtksourceregex.h			\
	gtksourcespacedrawer-private.h		\
	gtksourcestyle-private.h		\
	gtksourcetypes-private.h		\
	gtksourceundomanagerdefault.h		\
	gtksourceutils-private.h

libgtksourceview_private_c_files =	\
	gtksourcebufferinputstream.c	\
	gtksourcebufferinternal.c	\
	gtksourcebufferoutputstream.c	\
	gtksourcecompletioncontainer.c	\
	gtksourcecompletionmodel.c	\
	gtksourcecontextengine.c	\
	gtksourceengine.c		\
	gtksourcegutterrendererlines.c	\
	gtksourcegutterrenderermarks.c	\
	gtksourceiter.c			\
	gtksourcelanguage-parser-2.c	\
	gtksourcemarkssequence.c	\
	gtksourcepixbufhelper.c		\
	gtksourceregex.c		\
	gtksourceundomanagerdefault.c

# Split in a helper library, so the private functions can be used in unit tests.
noinst_LTLIBRARIES = libgtksourceview-core.la

libgtksourceview_core_la_SOURCES =		\
	$(libgtksourceview_public_c_files)	\
	$(libgtksourceview_public_headers)	\
	$(libgtksourceview_private_c_files)	\
	$(libgtksourceview_private_headers)

# do not distribute generated files
nodist_libgtksourceview_core_la_SOURCES = \
	$(BUILT_SOURCES)

libgtksourceview_core_la_CFLAGS =	\
	$(HIDDEN_VISIBILITY_CFLAGS)	\
	$(CODE_COVERAGE_CFLAGS)

libgtksourceview_core_la_LDFLAGS =	\
	-no-undefined 			\
	$(WARN_LDFLAGS)

libgtksourceview_core_la_LIBADD =	\
	$(CODE_COVERAGE_LIBS)

if OS_OSX
libgtksourceview_core_la_CFLAGS += -xobjective-c
libgtksourceview_core_la_LDFLAGS += -framework Cocoa
endif

# The real library
lib_LTLIBRARIES = libgtksourceview-@GSV_API_VERSION@.la

libgtksourceview_@GSV_API_VERSION@_la_SOURCES =

libgtksourceview_@GSV_API_VERSION@_la_LIBADD = 				\
	libgtksourceview-core.la					\
	completion-providers/words/libgtksourcecompletionwords.la	\
	$(CODE_COVERAGE_LIBS)						\
	$(LIBM)								\
	$(DEP_LIBS)							\
	$(GTK_MAC_LIBS)

libgtksourceview_@GSV_API_VERSION@_la_CFLAGS =	\
	$(HIDDEN_VISIBILITY_CFLAGS)		\
	$(CODE_COVERAGE_CFLAGS)

libgtksourceview_@GSV_API_VERSION@_la_LDFLAGS =	\
	-version-info $(GSV_LT_VERSION)		\
	-no-undefined 				\
	-export-dynamic				\
	$(WARN_LDFLAGS)

libgtksourceview_@GSV_API_VERSION@_includedir =				\
	$(includedir)/gtksourceview-@GSV_API_VERSION@/gtksourceview

libgtksourceview_@GSV_API_VERSION@_include_HEADERS = $(libgtksourceview_public_headers)

nodist_libgtksourceview_@GSV_API_VERSION@_include_HEADERS =	\
	gtksourceversion.h					\
	gtksource-enumtypes.h

gtksourceview-resources.c: gtksourceview.gresource.xml $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir) --generate-dependencies $(srcdir)/gtksourceview.gresource.xml)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --generate-source $(srcdir)/gtksourceview.gresource.xml

ENUM_TYPES = $(libgtksourceview_public_headers)

gtksource-enumtypes.h: gtksource-enumtypes.h.in $(ENUM_TYPES)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template gtksource-enumtypes.h.in --identifier-prefix GtkSource --symbol-prefix gtk_source $(ENUM_TYPES)) > $@

gtksource-enumtypes.c: gtksource-enumtypes.c.in $(ENUM_TYPES)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template gtksource-enumtypes.c.in --identifier-prefix GtkSource --symbol-prefix gtk_source $(ENUM_TYPES)) > $@

EXTRA_DIST = \
	gtksourcecompletion.ui \
	gtksourceview.gresource.xml \
	gtksource-enumtypes.c.in \
	gtksource-enumtypes.h.in

CLEANFILES = $(BUILT_SOURCES)

libgtksourcecompletionwords_files =	\
	./completion-providers/words/gtksourcecompletionwords.h	\
	./completion-providers/words/gtksourcecompletionwords.c

if HAVE_INTROSPECTION
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS = GtkSource-@GSV_API_VERSION@.gir

INTROSPECTION_SCANNER_ENV = CC="$(CC)"

GtkSource-@GSV_API_VERSION@.gir: libgtksourceview-@GSV_API_VERSION@.la $(BUILT_SOURCES)
INTROSPECTION_SCANNER_ARGS =			\
	-I$(top_srcdir)				\
	-I$(top_builddir)			\
	--c-include=gtksourceview/gtksource.h	\
	--warn-all
GtkSource_@GSV_API_VERSION@_gir_NAMESPACE = GtkSource
GtkSource_@GSV_API_VERSION@_gir_VERSION = @GSV_API_VERSION@
GtkSource_@GSV_API_VERSION@_gir_CFLAGS = $(DEP_CFLAGS)
GtkSource_@GSV_API_VERSION@_gir_SCANNERFLAGS = $(WARN_SCANNERFLAGS)
GtkSource_@GSV_API_VERSION@_gir_LIBS = libgtksourceview-@GSV_API_VERSION@.la

# Ensure that gtksource.h is the first one.
GtkSource_@GSV_API_VERSION@_gir_FILES =		\
	gtksource.h				\
	$(libgtksourcecompletionwords_files)	\
	$(libgtksourceview_public_headers)	\
	$(libgtksourceview_public_c_files)	\
	$(BUILT_SOURCES)

GtkSource_@GSV_API_VERSION@_gir_INCLUDES = Gtk-3.0 Gdk-3.0
GtkSource_@GSV_API_VERSION@_gir_PACKAGES = gtk+-3.0 gdk-3.0
GtkSource_@GSV_API_VERSION@_gir_EXPORT_PACKAGES = gtksourceview-@GSV_API_VERSION@

girdir = $(datadir)/gir-1.0
gir_DATA = GtkSource-@GSV_API_VERSION@.gir

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = GtkSource-@GSV_API_VERSION@.typelib

CLEANFILES +=		\
	$(gir_DATA)	\
	$(typelib_DATA)

endif # HAVE_INTROSPECTION

-include $(top_srcdir)/git.mk
