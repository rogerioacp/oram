AUTOMAKE_OPTIONS = subdir-objects

# extra flag to check for padding in data structures for aligned accesses
# https://wr.informatik.uni-hamburg.de/_media/teaching/wintersemester_2013_2014/epc-14-haase-svenhendrik-alignmentinc-paper.pdf
AM_CFLAGS = $(stash_count) -Wpadded
pkginclude_HEADERS = include/oram/plblock.h include/oram/logger.h include/oram/ofile.h include/oram/oram.h include/oram/pmap.h include/oram/stash.h include/oram/orandom.h include/oram/foram.h include/oram/pmapdefs/pdeforam.h include/oram/pmapdefs/fdeforam.h


pathoram_tests = singleread singlewrite singlereadwrite multiread multiwriteread randomwrites randomwriteread largerandomwrites largerandomwriteread optimalzread optimalzwrite optimalzreadwrite optimalzmultiread optimalzmultiwriteread optimalzrandomwrites optimalzrandomwriteread optimalzlargerandomwrites optimalzlargerandomwriteread

forestoram_tests = singlereadf singlewritef singlereadwritef multireadf multiwritereadf randomwritesf randomwritereadf largerandomwritesf largerandomwritereadf optimalzreadf optimalzwritef optimalzreadwritef optimalzmultireadf optimalzmultiwritereadf optimalzrandomwritesf optimalzrandomwritereadf optimalzlargerandomwritesf optimalzlargerandomwritereadf

doubleobliv_tests = optimalzreaddouble optimalzwritedouble optimalzreadwritedouble optimalzmultireaddouble optimalzmultiwritereaddouble optimalzrandomwritesdouble optimalzrandomwritereaddouble optimalzlargerandomwritesdouble optimalzlargerandomwritereaddouble

doubleoblivf_tests = optimalzreaddoublef optimalzwritedoublef optimalzreadwritedoublef optimalzmultireaddoublef optimalzmultiwritereaddoublef optimalzrandomwritesdoublef optimalzrandomwritereaddoublef optimalzlargerandomwritesdoublef optimalzlargerandomwritereaddoublef

#bin_PROGRAMS = read write readwrite
check_PROGRAMS = $(pathoram_tests) $(forestoram_tests) $(doubleobliv_tests) $(doubleoblivf_tests)
#check_PROGRAMS = $(doubleobliv_tests)


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

memory_test_files_d = backend/logger/logger.c backend/ofile/ofile.c backend/pmap/pmap.c backend/stash/dstash.c backend/block/plblock.c

memory_test_files_df = backend/logger/logger.c backend/ofile/ofile.c backend/pmap/fpmap.c backend/stash/dstash.c backend/block/plblock.c

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

optimalzread_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/optimalz_read.c
optimalzread_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzread_LDADD = $(COLLECTC_LIBS)

optimalzreadf_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/optimalz_read.c
optimalzreadf_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzreadf_LDADD = $(COLLECTC_LIBS)

optimalzwrite_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/optimalz_write.c
optimalzwrite_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzwrite_LDADD = $(COLLECTC_LIBS)

optimalzwritef_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/optimalz_write.c
optimalzwritef_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzwritef_LDADD = $(COLLECTC_LIBS)

optimalzreadwrite_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/optimalz_readwrite.c
optimalzreadwrite_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzreadwrite_LDADD = $(COLLECTC_LIBS)

optimalzreadwritef_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/optimalz_readwrite.c
optimalzreadwritef_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzreadwritef_LDADD = $(COLLECTC_LIBS)

optimalzmultiread_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/optimalz_multiread.c
optimalzmultiread_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzmultiread_LDADD = $(COLLECTC_LIBS)

optimalzmultireadf_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/optimalz_multiread.c
optimalzmultireadf_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzmultireadf_LDADD = $(COLLECTC_LIBS)

optimalzmultiwriteread_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/optimalz_multiwriteread.c
optimalzmultiwriteread_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzmultiwriteread_LDADD = $(COLLECTC_LIBS)

optimalzmultiwritereadf_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/optimalz_multiwriteread.c
optimalzmultiwritereadf_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzmultiwritereadf_LDADD = $(COLLECTC_LIBS)

optimalzrandomwrites_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/optimalz_randomwrites.c
optimalzrandomwrites_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzrandomwrites_LDADD = $(COLLECTC_LIBS)

optimalzrandomwritesf_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/optimalz_randomwrites.c
optimalzrandomwritesf_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzrandomwritesf_LDADD = $(COLLECTC_LIBS)

optimalzrandomwriteread_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/optimalz_randomwrites.c
optimalzrandomwriteread_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzrandomwriteread_LDADD = $(COLLECTC_LIBS)

optimalzrandomwritereadf_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/optimalz_randomwrites.c
optimalzrandomwritereadf_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzrandomwritereadf_LDADD = $(COLLECTC_LIBS)

optimalzlargerandomwrites_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/optimalz_largerandomwrites.c
optimalzlargerandomwrites_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzlargerandomwrites_LDADD = $(COLLECTC_LIBS)

optimalzlargerandomwritesf_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/optimalz_largerandomwrites.c
optimalzlargerandomwritesf_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzlargerandomwritesf_LDADD = $(COLLECTC_LIBS)

optimalzlargerandomwriteread_SOURCES = backend/oram/pathoram.c $(memory_test_files) $(random_file) tests/optimalz_large_randomwriteread.c
optimalzlargerandomwriteread_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzlargerandomwriteread_LDADD = $(COLLECTC_LIBS)

optimalzlargerandomwritereadf_SOURCES = backend/oram/forestoram.c $(memory_test_files_f) $(random_file) tests/optimalz_large_randomwriteread.c
optimalzlargerandomwritereadf_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzlargerandomwritereadf_LDADD = $(COLLECTC_LIBS)


# Double oblivious optimal configuration Path ORAM

optimalzreaddouble_SOURCES = backend/oram/pathoram.c $(memory_test_files_d) $(random_file) tests/optimalz_read.c
optimalzreaddouble_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzreaddouble_LDADD = $(COLLECTC_LIBS)

optimalzwritedouble_SOURCES = backend/oram/pathoram.c $(memory_test_files_d) $(random_file) tests/optimalz_write.c
optimalzwritedouble_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzwritedouble_LDADD = $(COLLECTC_LIBS)

optimalzreadwritedouble_SOURCES = backend/oram/pathoram.c $(memory_test_files_d) $(random_file) tests/optimalz_readwrite.c
optimalzreadwritedouble_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzreadwritedouble_LDADD = $(COLLECTC_LIBS)

optimalzmultireaddouble_SOURCES = backend/oram/pathoram.c $(memory_test_files_d) $(random_file) tests/optimalz_multiread.c
optimalzmultireaddouble_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzmultireaddouble_LDADD = $(COLLECTC_LIBS)

optimalzmultiwritereaddouble_SOURCES = backend/oram/pathoram.c $(memory_test_files_d) $(random_file) tests/optimalz_multiwriteread.c
optimalzmultiwritereaddouble_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzmultiwritereaddouble_LDADD = $(COLLECTC_LIBS)

optimalzrandomwritesdouble_SOURCES = backend/oram/pathoram.c $(memory_test_files_d) $(random_file) tests/optimalz_randomwrites.c
optimalzrandomwritesdouble_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzrandomwritesdouble_LDADD = $(COLLECTC_LIBS)

optimalzrandomwritereaddouble_SOURCES = backend/oram/pathoram.c $(memory_test_files_d) $(random_file) tests/optimalz_randomwrites.c
optimalzrandomwritereaddouble_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzrandomwritereaddouble_LDADD = $(COLLECTC_LIBS)

optimalzlargerandomwritesdouble_SOURCES = backend/oram/pathoram.c $(memory_test_files_d) $(random_file) tests/optimalz_largerandomwrites.c
optimalzlargerandomwritesdouble_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzlargerandomwritesdouble_LDADD = $(COLLECTC_LIBS)

optimalzlargerandomwritereaddouble_SOURCES = backend/oram/pathoram.c $(memory_test_files_d) $(random_file) tests/optimalz_large_randomwriteread.c
optimalzlargerandomwritereaddouble_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzlargerandomwritereaddouble_LDADD = $(COLLECTC_LIBS)


# Double oblivious optimal configuration Forest ORAM

optimalzreaddoublef_SOURCES = backend/oram/forestoram.c $(memory_test_files_df) $(random_file) tests/optimalz_read.c
optimalzreaddoublef_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzreaddoublef_LDADD = $(COLLECTC_LIBS)

optimalzwritedoublef_SOURCES = backend/oram/forestoram.c $(memory_test_files_df) $(random_file) tests/optimalz_write.c
optimalzwritedoublef_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzwritedoublef_LDADD = $(COLLECTC_LIBS)

optimalzreadwritedoublef_SOURCES = backend/oram/forestoram.c $(memory_test_files_df) $(random_file) tests/optimalz_readwrite.c
optimalzreadwritedoublef_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzreadwritedoublef_LDADD = $(COLLECTC_LIBS)

optimalzmultireaddoublef_SOURCES = backend/oram/forestoram.c $(memory_test_files_df) $(random_file) tests/optimalz_multiread.c
optimalzmultireaddoublef_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzmultireaddoublef_LDADD = $(COLLECTC_LIBS)

optimalzmultiwritereaddoublef_SOURCES = backend/oram/forestoram.c $(memory_test_files_df) $(random_file) tests/optimalz_multiwriteread.c
optimalzmultiwritereaddoublef_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzmultiwritereaddoublef_LDADD = $(COLLECTC_LIBS)

optimalzrandomwritesdoublef_SOURCES = backend/oram/forestoram.c $(memory_test_files_df) $(random_file) tests/optimalz_randomwrites.c
optimalzrandomwritesdoublef_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzrandomwritesdoublef_LDADD = $(COLLECTC_LIBS)

optimalzrandomwritereaddoublef_SOURCES = backend/oram/forestoram.c $(memory_test_files_df) $(random_file) tests/optimalz_randomwrites.c
optimalzrandomwritereaddoublef_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzrandomwritereaddoublef_LDADD = $(COLLECTC_LIBS)

optimalzlargerandomwritesdoublef_SOURCES = backend/oram/forestoram.c $(memory_test_files_df) $(random_file) tests/optimalz_largerandomwrites.c
optimalzlargerandomwritesdoublef_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzlargerandomwritesdoublef_LDADD = $(COLLECTC_LIBS)

optimalzlargerandomwritereaddoublef_SOURCES = backend/oram/forestoram.c $(memory_test_files_df) $(random_file) tests/optimalz_large_randomwriteread.c
optimalzlargerandomwritereaddoublef_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
optimalzlargerandomwritereaddoublef_LDADD = $(COLLECTC_LIBS)



TESTS = $(check_PROGRAMS)

lib_LTLIBRARIES = libpathoram.la libforestoram.la

libpathoram_la_SOURCES =  backend/ofile/ofile.c backend/pmap/pmap.c backend/stash/stash.c backend/block/plblock.c backend/oram/pathoram.c
libpathoram_la_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
libpathoram_la_LIBADD = $(COLLECTC_LIBS)

libforestoram_la_SOURCES =  backend/ofile/ofile.c backend/pmap/fpmap.c backend/stash/stash.c backend/block/plblock.c backend/oram/forestoram.c
libforestoram_la_CFLAGS = $(stash_count) $(COLLECTC_CFLAGS) -I $(srcdir)/include
libforestoram_la_LIBADD = $(COLLECTC_LIBS)
