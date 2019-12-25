AUTOMAKE_OPTIONS = subdir-objects

# extra flag to check for padding in data structures for aligned accesses
# https://wr.informatik.uni-hamburg.de/_media/teaching/wintersemester_2013_2014/epc-14-haase-svenhendrik-alignmentinc-paper.pdf
AM_CFLAGS = $(stash_count) -Wpadded
pkginclude_HEADERS = include/oram/plblock.h include/oram/logger.h include/oram/ofile.h include/oram/oram.h include/oram/pmap.h include/oram/stash.h include/oram/orandom.h include/oram/foram.h include/oram/pmapdefs/pdeforam.h include/oram/pmapdefs/fdeforam.h


pathoram_tests = singleread singlewrite singlereadwrite multiread multiwriteread randomwrites randomwriteread largerandomwrites largerandomwriteread 

forestoram_tests = singlereadf singlewritef singlereadwritef multireadf multiwritereadf randomwritesf randomwritereadf largerandomwritesf largerandomwritereadf


#bin_PROGRAMS = read write readwrite
check_PROGRAMS = $(pathoram_tests) $(forestoram_tests)


if SCOUNT
     stash_count = -DSTASH_COUNT
else
     stash_count = -USTASH_COUNT 
endif

if LINUX
     random_file = backend/orandom/linux_random.c
else
     random_file = backend/orandom/bsd_random.c
endif


if SFORAM
     stash_count += -DSFORAM
endif


memory_test_files = backend/logger/logger.c backend/ofile/ofile.c backend/pmap/pmap.c backend/stash/stash.c backend/block/plblock.c

memory_test_files_f = backend/logger/logger.c backend/ofile/ofile.c backend/pmap/fpmap.c backend/stash/stash.c backend/block/plblock.c


singleread_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/singleread.c
singleread_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include  
singleread_LDADD = $(COLLECTC_LIBS)

singlereadf_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/singleread.c
singlereadf_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
singlereadf_LDADD = $(COLLECTC_LIBS)


singlewrite_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/singlewrite.c
singlewrite_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include  
singlewrite_LDADD = $(COLLECTC_LIBS)

singlewritef_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/singlewrite.c
singlewritef_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include  
singlewritef_LDADD = $(COLLECTC_LIBS) 

singlereadwrite_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/singlereadwrite.c
singlereadwrite_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include  
singlereadwrite_LDADD = $(COLLECTC_LIBS)

singlereadwritef_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/singlereadwrite.c
singlereadwritef_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include  
singlereadwritef_LDADD = $(COLLECTC_LIBS)

multiread_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/multiread.c
multiread_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include  
multiread_LDADD = $(COLLECTC_LIBS)


multireadf_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/multiread.c
multireadf_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include  
multireadf_LDADD = $(COLLECTC_LIBS)

multiwriteread_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/multiwriteread.c
multiwriteread_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include  
multiwriteread_LDADD = $(COLLECTC_LIBS)


multiwritereadf_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/multiwriteread.c
multiwritereadf_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include  
multiwritereadf_LDADD = $(COLLECTC_LIBS)


randomwrites_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/randomwrites.c
randomwrites_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
randomwrites_LDADD = $(COLLECTC_LIBS)


randomwritesf_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/randomwrites.c
randomwritesf_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
randomwritesf_LDADD = $(COLLECTC_LIBS)


randomwriteread_SOURCES = backend/oram/pathoram.c $(memory_test_files)  $(random_file) tests/randomwriteread.c
randomwriteread_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
randomwriteread_LDADD = $(COLLECTC_LIBS)

randomwritereadf_SOURCES = backend/oram/forestoram.c $(memory_test_files_f)  $(random_file) tests/randomwriteread.c
randomwritereadf_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
randomwritereadf_LDADD = $(COLLECTC_LIBS)


largerandomwrites_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/large_randomwrites.c
largerandomwrites_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
largerandomwrites_LDADD = $(COLLECTC_LIBS)


largerandomwritesf_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/large_randomwrites.c
largerandomwritesf_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
largerandomwritesf_LDADD = $(COLLECTC_LIBS)


largerandomwriteread_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/large_randomwriteread.c
largerandomwriteread_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
largerandomwriteread_LDADD = $(COLLECTC_LIBS)


largerandomwritereadf_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/large_randomwriteread.c
largerandomwritereadf_CFLAGS = $(stash_count)  $(COLLECTC_CFLAGS) -I $(srcdir)/include
largerandomwritereadf_LDADD = $(COLLECTC_LIBS)

TESTS = $(check_PROGRAMS)

lib_LTLIBRARIES = libpathoram.la libforestoram.la

libpathoram_la_SOURCES =  backend/ofile/ofile.c backend/pmap/pmap.c backend/stash/stash.c backend/block/plblock.c backend/oram/pathoram.c
libpathoram_la_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
libpathoram_la_LIBADD = $(COLLECTC_LIBS)

libforestoram_la_SOURCES =  backend/ofile/ofile.c backend/pmap/fpmap.c backend/stash/stash.c backend/block/plblock.c backend/oram/forestoram.c
libforestoram_la_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
libforestoram_la_LIBADD = $(COLLECTC_LIBS)
