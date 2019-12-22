AC_INIT([oram], [0.0.1], [rogerio.pontes@pm.me])

AM_INIT_AUTOMAKE([-Wall -Werror foreign]) 
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
    src/Makefile
    ])

AM_PROG_AR
LT_INIT

AC_CONFIG_MACRO_DIRS([m4])

# used to find packages installed on the system external to the standard library
# generates specific variables that can be used to include in specific targets.
PKG_CHECK_MODULES(COLLECTC, collectionc >= 3.0)

# used to include system libraries such as math.
# Includes in a global variable common to all targets.
AC_SEARCH_LIBS([pow], [m])

AM_SILENT_RULES([yes])


AC_CANONICAL_HOST

build_linux=no
build_windows=no
build_mac=no
build_scount=no

# Detect the target system
case "${host}" in
    *linux*)
        build_linux=yes
        ;;
    cygwin*|mingw*)
        build_windows=yes
        ;;
    *darwin*)
        build_mac=yes
        ;;
    *)
        AC_MSG_ERROR(["OS $host is not supported"])
        ;;
esac


AC_ARG_ENABLE([stash-count], 
               AS_HELP_STRING([--enable-stash-count], [Enables a preprocessing
                             directive that counts the number of blocks in each
                             stash of Forest ORAM and Path ORAM. This is a
                             debuging feature that should not be activated by
                             default]), [build_scount=yes],[stash_scount=no])

AM_CONDITIONAL([SCOUNT], [test "$build_scount" = "yes"])


# Pass the conditionals to automake
AM_CONDITIONAL([LINUX], [test "$build_linux" = "yes"])
AM_CONDITIONAL([WINDOWS], [test "$build_windows" = "yes"])
AM_CONDITIONAL([OSX], [test "$build_mac" = "yes"])



AC_OUTPUT
