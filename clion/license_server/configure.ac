#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([lightjbl],[0.2],[privatelo@noreply.com])

AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR(auto)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 dist-bzip2 tar-ustar foreign subdir-objects])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_INSTALL
AC_DISABLE_SHARED
AC_PROG_LIBTOOL
LT_INIT

# Checks for libev
m4_include([libev/libev.m4])

# Checks for header files.
AC_CHECK_HEADERS([sys/types.h stddef.h fcntl.h netdb.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for MinGW build.
case "$host" in
    *-mingw*)
        WIN32=yes
        LIBS="${LIBS} -lgdi32 -lws2_32"
        AC_CHECK_HEADERS([windows.h winsock2.h ws2tcpip.h],[],[AC_MSG_ERROR([Windows headers not found.])],[])
        # Add custom macros for libev
        AC_DEFINE([FD_SETSIZE], [2048], [Reset max file descriptor size.])
        AC_DEFINE([EV_FD_TO_WIN32_HANDLE(fd)], [(fd)], [Override libev default fd conversion macro.])
        AC_DEFINE([EV_WIN32_HANDLE_TO_FD(handle)], [(handle)], [Override libev default handle conversion macro.])
        AC_DEFINE([EV_WIN32_CLOSE_FD(fd)], [closesocket(fd)], [Override libev default fd close macro.])
        ;;
esac
AM_CONDITIONAL([WIN32], [test "${WIN32}" = "yes"])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
# To fix rpl_malloc undefined error in mips cross-compile enviroment.
AC_CHECK_FUNCS([malloc realloc])
AC_CHECK_FUNCS([select socket strstr memcpy])

# Check for select() into ws2_32 for MinGW
if test "$ac_cv_func_select" != "yes"; then
  AC_MSG_CHECKING([for select in ws2_32])
  AC_TRY_LINK([
#ifdef HAVE_WINSOCK2_H
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#endif
    ],[
      select(0,(fd_set *)NULL,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *)NULL);
    ],[
      AC_MSG_RESULT([yes])
      HAVE_SELECT="1"
      AC_DEFINE_UNQUOTED(HAVE_SELECT, 1,
        [Define to 1 if you have the 'select' function.])
      HAVE_SYS_SELECT_H="1"
      AC_DEFINE_UNQUOTED(HAVE_SYS_SELECT_H, 1,
        [Define to 1 if you have the <sys/select.h> header file.])
    ],[
      AC_MSG_ERROR([no])
  ])
fi

AC_ARG_ENABLE([debug],
    [  --enable-debug          build with additional debugging code],
    [CFLAGS="-g -O0 -DDEBUG"])

AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

AC_CONFIG_FILES([Makefile libev/Makefile src/Makefile])
AC_OUTPUT
