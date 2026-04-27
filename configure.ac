AC_PREREQ([2.68])
AC_INIT([libvmod-zstd], [trunk], [thomas.klinger@otto.de], [vmod-waf])
AC_COPYRIGHT([Copyright 2026 Thomas Klinger])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11 -Wall -Werror foreign parallel-tests])
AM_SILENT_RULES([yes])

AC_ARG_VAR([RST2MAN], [the program to build manuals from reStructuredText])
AC_CHECK_PROGS(RST2MAN, [rst2man rst2man.py])

VARNISH_PREREQ([7.5.0])
VARNISH_VMODS([zstd])

AM_PROG_AR

LT_PREREQ([2.2.6])
LT_INIT([dlopen disable-static])

AC_CHECK_HEADERS([stdatomic.h])

AC_CHECK_HEADERS([cache/cache.h], [],
    [AC_MSG_ERROR([Missing \$VARNISHSRC development files.])],
       [[#include <cache/cache.h>]])

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([ZSTD], [libzstd])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
])

AC_OUTPUT

AS_ECHO("
	==== $PACKAGE_STRING ====

	varnish:      $VARNISH_VERSION
	prefix:       $prefix
	vmoddir:      $vmoddir
	vcldir:       $vcldir
	pkgvcldir:    $pkgvcldir

	compiler:     $CC
	cflags:       $CFLAGS
	ldflags:      $LDFLAGS
")
