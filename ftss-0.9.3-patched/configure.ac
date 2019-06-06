AC_INIT(ftss, esyscmd([tr -d '\n' <VERSION]), mopo@fabletech.com)
AC_COPYRIGHT([Copyright (C) 2009 FableTech])

VERSION=$PACKAGE_VERSION
AC_SUBST(VERSION)

AM_INIT_AUTOMAKE(ftss, $VERSION)
AC_CONFIG_SRCDIR([src/ftss.c])
#AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

AC_LIB_PREFIX

dnl PKG_PROG_PKG_CONFIG

dnl PKG_CHECK_MODULES(LIBAPR1,[apr-1])


dnl ----- libapr -----

AC_ARG_WITH(apr_config,
		AS_HELP_STRING([--with-apr-config=program-path],
				   [Specify location of Apache Portable Runtime config tool]),,
		[with_apr_config=default])
APR_PATH="$PATH"
APR_PATH="$APR_PATH:/opt/hpws/apache/bin"
APR_PATH="$APR_PATH:/opt/IBMIHS/bin:/usr/IBMIHS/bin:/opt/freeware/bin"
APR_PATH="$APR_PATH:/usr/apache2/bin"
APR_PATH="$APR_PATH:/usr/local/bin:/usr/sbin:/usr/bin"
APR_PATH="$APR_PATH:/usr/local/apache2:/usr/local/apache"

if test x"$with_apr_config" = x"default"; then
	AC_PATH_PROG([APR0_CONFIG], [apr-config], [no], [$APR_PATH])
	if test x"$APR0_CONFIG" = x"no"; then
		AC_PATH_PROG([APR_CONFIG], [apr-1-config], [no], [$APR_PATH])
	else
		APR_CONFIG="$APR0_CONFIG"
	fi
else
	APR_CONFIG="$with_apr_config"
fi
if test x"$APR_CONFIG" = x"no"; then
	AC_MSG_FAILURE([no apr-config found. Is the Apache developer kit installed?])
fi

dnl -- APR CFLAGS
AC_CACHE_CHECK([APR CFLAGS], [ftss_cv_libapr_cflags],
	[ftss_cv_libapr_cflags=`$APR_CONFIG --cflags --cppflags --includes`])
CFLAGS="$CFLAGS $ftss_cv_libapr_cflags"

dnl -- APR LIBS
AC_CACHE_CHECK([APR LIBS], [ftss_cv_libapr_libs],
	[ftss_cv_libapr_libs=`$APR_CONFIG --link-ld`])
LIBS="$LIBS $ftss_cv_libapr_libs"


dnl ----- apxs -----

dnl -- APXS is Apache's extension tool.
dnl	We trust it completely to do the right thing.
dnl	If it is busted, then the user is on their own.
AC_ARG_WITH(apxs,
		AS_HELP_STRING([--with-apxs=program-path],
				   [Specify location of Apache extension tool]),,
		[with_apxs=default])
dnl -- Search path for finding apxs2/apxs
APXS_PATH="$PATH"
APXS_PATH="$APXS_PATH:/opt/hpws/apache/bin"
APXS_PATH="$APXS_PATH:/opt/IBMIHS/bin:/usr/IBMIHS/bin:/opt/freeware/bin"
APXS_PATH="$APXS_PATH:/usr/apache2/bin"
APXS_PATH="$APXS_PATH:/usr/local/bin:/usr/sbin:/usr/bin"
APXS_PATH="$APXS_PATH:/usr/local/apache2:/usr/local/apache"

if test x"$with_apxs" = x"default"; then
	AC_PATH_PROG([APXS2], [apxs2], [no], [$APXS_PATH])
	if test x"$APXS2" = x"no"; then
		AC_PATH_PROG([APXS], [apxs], [no], [$APXS_PATH])
	else
		APXS="$APXS2"
	fi
else
	APXS="$with_apxs"
fi
if test x"$APXS" = x"no"; then
	AC_MSG_FAILURE([no apxs found. Is the Apache developer kit installed?])
fi
AC_SUBST(APXS)

dnl -- use this as a sanity test that apxs is working
AC_MSG_CHECKING([Apache extension target])
apxs_target=`$APXS -q TARGET || echo error`
AC_MSG_RESULT([$apxs_target])

if test x"$apxs_target" = x"error"; then
	AC_MSG_FAILURE([$APXS does not appear to work])
fi

dnl -- APXS CFLAGS
AC_CACHE_CHECK([APXS CFLAGS], [ftss_cv_apxs_cflags],
	[ftss_cv_apxs_cflags=`$APXS -q CFLAGS`])
CFLAGS="$CFLAGS $ftss_cv_apxs_cflags"

dnl -- APXS INCLUDEDIR
AC_CACHE_CHECK([APXS INCLUDEDIR], [ftss_cv_apxs_includedir],
	[ftss_cv_apxs_includedir=`$APXS -q INCLUDEDIR`])
CFLAGS="$CFLAGS -I$ftss_cv_apxs_includedir"


AC_MSG_CHECKING([for load balancer support in mod_status])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include "httpd.h"
#include "scoreboard.h"
]], [[
scoreboard sb;
if (sb.balancers) return 0
]])],
[
	AC_MSG_RESULT([yes])
	CFLAGS="$CFLAGS -DSCOREBOARD_BALANCERS"
], [
        AC_MSG_RESULT([no])
])


AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_OUTPUT
