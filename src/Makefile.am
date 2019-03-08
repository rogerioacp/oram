AUTOMAKE_OPTIONS = subdir-objects

include_HEADERS = include/block.h include/logger.h include/ofile.h include/oram.h include/pmap.h include/stash.h

#bin_PROGRAMS = read write readwrite
check_PROGRAMS = singleread singlewrite singlereadwrite multiread multiwriteread randomwrites randomwriteread largerandomwrites largerandomwriteread

memory_test_files = backend/logger/logger.c backend/ofile/ofile.c backend/pmap/pmap.c backend/stash/stash.c backend/block/block.c

singleread_SOURCES = backend/oram/pathoram.c $(memory_test_files) tests/singleread.c
singleread_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include  
singleread_LDFLAGS = $(GLIB_LIBS)

singlewrite_SOURCES = backend/oram/pathoram.c $(memory_test_files) tests/singlewrite.c
singlewrite_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include  
singlewrite_LDFLAGS = $(GLIB_LIBS)

singlereadwrite_SOURCES = backend/oram/pathoram.c $(memory_test_files) tests/singlereadwrite.c
singlereadwrite_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include  
singlereadwrite_LDFLAGS = $(GLIB_LIBS)

multiread_SOURCES = backend/oram/pathoram.c $(memory_test_files) tests/multiread.c
multiread_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include  
multiread_LDFLAGS = $(GLIB_LIBS)

multiwriteread_SOURCES = backend/oram/pathoram.c $(memory_test_files) tests/multiwriteread.c
multiwriteread_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include  
multiwriteread_LDFLAGS = $(GLIB_LIBS)

randomwrites_SOURCES = backend/oram/pathoram.c $(memory_test_files) tests/randomwrites.c
randomwrites_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include
randomwrites_LDFLAGS = $(GLIB_LIBS)

randomwriteread_SOURCES = backend/oram/pathoram.c $(memory_test_files) tests/randomwriteread.c
randomwriteread_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include
randomwriteread_LDFLAGS = $(GLIB_LIBS)

largerandomwrites_SOURCES = backend/oram/pathoram.c $(memory_test_files) tests/large_randomwrites.c
largerandomwrites_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include
largerandomwrites_LDFLAGS = $(GLIB_LIBS)

largerandomwriteread_SOURCES = backend/oram/pathoram.c $(memory_test_files) tests/large_randomwriteread.c
largerandomwriteread_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include
largerandomwriteread_LDFLAGS = $(GLIB_LIBS)

TESTS = $(check_PROGRAMS)