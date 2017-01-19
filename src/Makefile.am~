AUTOMAKE_OPTIONS = foreign
bin_PROGRAMS = $(top_builddir)/bin/cvs-init $(top_builddir)/bin/cvs-write_tree $(top_builddir)/bin/cvs-update $(top_builddir)/bin/cvs-diff $(top_builddir)/bin/cvs-cat $(top_builddir)/bin/cvs-commit $(top_builddir)/bin/cvs-read_tree $(top_builddir)/bin/cvs-log $(top_builddir)/bin/cvs-status
AM_CPPFLAGS = -I$(top_builddir)/include 

__top_builddir__bin_cvs_status_LDADD= -lssl -lz -lcrypto
__top_builddir__bin_cvs_read_tree_LDADD= -lssl -lz -lcrypto
__top_builddir__bin_cvs_log_LDADD= -lssl -lz -lcrypto
__top_builddir__bin_cvs_init_LDADD= -lssl -lz -lcrypto
__top_builddir__bin_cvs_cat_LDADD= -lssl -lz -lcrypto
__top_builddir__bin_cvs_commit_LDADD= -lssl -lz -lcrypto
__top_builddir__bin_cvs_diff_LDADD= -lssl -lz -lcrypto
__top_builddir__bin_cvs_write_tree_LDADD= -lssl -lz -lcrypto
__top_builddir__bin_cvs_update_LDADD= -lssl -lz -lcrypto

__top_builddir__bin_cvs_status_SOURCES = cvs-status.c cvs-read.c cache.h
__top_builddir__bin_cvs_log_SOURCES = cvs-log.c cvs-read.c cache.h
__top_builddir__bin_cvs_read_tree_SOURCES = cvs-read_tree.c cvs-read.c cache.h
__top_builddir__bin_cvs_init_SOURCES = cvs-init.c cvs-read.c cache.h
__top_builddir__bin_cvs_diff_SOURCES = cvs-diff.c cvs-read.c cache.h
__top_builddir__bin_cvs_cat_SOURCES = cvs-cat.c cvs-read.c cache.h
__top_builddir__bin_cvs_commit_SOURCES = cvs-commit.c cvs-read.c cache.h
__top_builddir__bin_cvs_write_tree_SOURCES = cvs-write_tree.c cvs-read.c  cache.h
__top_builddir__bin_cvs_update_SOURCES = cvs-update.c cvs-read.c  cache.h

