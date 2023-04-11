SUBDIRS = po data gtksourceview tests testsuite docs

include $(top_srcdir)/aminclude_static.am
clean-local: code-coverage-clean
distclean-local: code-coverage-dist-clean

AM_DISTCHECK_CONFIGURE_FLAGS =		\
	--enable-gtk-doc		\
	--enable-introspection		\
	--disable-code-coverage

pkgconfig_DATA = libgedit-gtksourceview-@GSV_API_VERSION@.pc

EXTRA_DIST = \
	HACKING \
	libgedit-gtksourceview.pc.in

MAINTAINERCLEANFILES = \
	$(GITIGNORE_MAINTAINERCLEANFILES_TOPLEVEL) \
	$(GITIGNORE_MAINTAINERCLEANFILES_MAKEFILE_IN) \
	$(GITIGNORE_MAINTAINERCLEANFILES_M4_LIBTOOL) \
	aminclude_static.am \
	gtk-doc.make \
	INSTALL

-include $(top_srcdir)/git.mk
