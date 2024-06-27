AC_INIT([gpv-eink], [0.1], [graeme.waveshare@vetterlein.com])
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
AC_PROG_RANLIB([ranlib])
AM_PROG_AR([ar])
AC_OUTPUT
