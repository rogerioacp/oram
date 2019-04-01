AUTOMAKE_OPTIONS = subdir-objects

# extra flag to check for padding in data structures for aligned accesses
# https://wr.informatik.uni-hamburg.de/_media/teaching/wintersemester_2013_2014/epc-14-haase-svenhendrik-alignmentinc-paper.pdf
AM_CFLAGS = -Wpadded
pkginclude_HEADERS = include/oram/plblock.h include/oram/logger.h include/oram/ofile.h include/oram/oram.h include/oram/pmap.h include/oram/stash.h include/oram/orandom.h

#bin_PROGRAMS = read write readwrite
check_PROGRAMS = singleread singlewrite singlereadwrite multiread multiwriteread randomwrites randomwriteread largerandomwrites largerandomwriteread

if LINUX
     random_file = backend/orandom/linux_random.c
else
     random_file = backend/orandom/bsd_random.c
endif

memory_test_files = backend/logger/logger.c backend/ofile/ofile.c backend/pmap/pmap.c backend/stash/stash.c backend/block/plblock.c

singleread_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/singleread.c
singleread_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include  
singleread_LDADD = $(GLIB_LIBS)

singlewrite_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/singlewrite.c
singlewrite_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include  
singlewrite_LDADD = $(GLIB_LIBS)

singlereadwrite_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/singlereadwrite.c
singlereadwrite_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include  
singlereadwrite_LDADD = $(GLIB_LIBS)

multiread_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/multiread.c
multiread_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include  
multiread_LDADD = $(GLIB_LIBS)

multiwriteread_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/multiwriteread.c
multiwriteread_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include  
multiwriteread_LDADD = $(GLIB_LIBS)

randomwrites_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/randomwrites.c
randomwrites_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include
randomwrites_LDADD = $(GLIB_LIBS)

randomwriteread_SOURCES = backend/oram/pathoram.c $(memory_test_files)  $(random_file) tests/randomwriteread.c
randomwriteread_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include
randomwriteread_LDADD = $(GLIB_LIBS)

largerandomwrites_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/large_randomwrites.c
largerandomwrites_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include
largerandomwrites_LDADD = $(GLIB_LIBS)

largerandomwriteread_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/large_randomwriteread.c
largerandomwriteread_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include
largerandomwriteread_LDADD = $(GLIB_LIBS)

TESTS = $(check_PROGRAMS)

lib_LTLIBRARIES = liboram.la
liboram_la_SOURCES = backend/logger/logger.c backend/ofile/ofile.c backend/pmap/pmap.c backend/stash/stash.c backend/block/plblock.c $(random_file) backend/oram/pathoram.c
liboram_la_CFLAGS = $(GLIB_CFLAGS) -I $(srcdir)/include
liboram_la_LIBADD = $(GLIB_LIBS)