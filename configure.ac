AC_INIT([einksysstat], [m4_esyscmd_s([git describe --abbrev=7 --dirty --always --tags])], [graeme.waveshare@vetterlein.com])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_DEFINE([RPI], [], [Build is for Raspberry Pi])
AC_DEFINE([DEBUG], [1], [Debug macro is active])
AC_DEFINE([USE_LGPIO_LIB], [1], [Use -lgpio rather than previous options])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 Device/Makefile
 GUI/Makefile
 Fonts/Makefile
])
#AC_PROG_RANLIB([ranlib])
AC_PROG_RANLIB
AC_PROG_LEX([noyywrap])

#AM_PROG_AR([ar])
AM_PROG_AR
#AM_PROG_LEX([flex])
AM_PROG_LEX
#AC_PROG_YACC([bison])
AC_PROG_YACC
AC_OUTPUT

