#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([bitmailcore], [1.0], [imharrywu@gmail.com])
AC_CONFIG_SRCDIR([src/bitmail.cpp])
#AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE() 
LT_INIT([win32-dll])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strdup])

AC_CONFIG_FILES(Makefile
		src/Makefile
        include/Makefile
        include/bitmailcore/Makefile)

AC_OUTPUT
