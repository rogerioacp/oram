AUTOMAKE_OPTIONS = subdir-objects



AM_CFLAGS= -I $(srcdir)/include  

include_HEADERS= include/block.h include/logger.h include/ofile.h include/oram.h include/pmap.h include/stash.h

bin_PROGRAMS = memory


memory_SOURCES = backend/oram/pathoram.c backend/logger/logger.c backend/ofile/ofile.c backend/pmap/pmap.c backend/stash/stash.c tests/memory.c
memory_CFLAGS = $(GLIB)
memory_LDFLAGS= $(GLIB)


TESTS= memory
