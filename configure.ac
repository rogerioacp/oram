AC_INIT([oram], [1.0], [rogeriocap@google.com])



AM_INIT_AUTOMAKE([-Wall -Werror foreign]) 
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
    src/Makefile
    ])

# used to find packages installed on the system external to the standard library
# generates specific variables that can be used to include in specific targets.
PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.4)

# used to include system libraries such as math.
# Includes in a global variable common to all targets.
AC_SEARCH_LIBS([pow], [m])

build_linux=no
build_windows=no
build_mac=no

# Detect the target system
case "${host}" in
    linux*)
        build_linux=yes
        ;;
    cygwin*|mingw*)
        build_windows=yes
        ;;
    darwin*)
        build_mac=yes
        ;;
    *)
        AC_MSG_ERROR(["OS $host is not supported"])
        ;;
esac

# Pass the conditionals to automake
AM_CONDITIONAL([LINUX], [test "$build_linux" = "yes"])
AM_CONDITIONAL([WINDOWS], [test "$build_windows" = "yes"])
AM_CONDITIONAL([OSX], [test "$build_mac" = "yes"])
AC_OUTPUT
