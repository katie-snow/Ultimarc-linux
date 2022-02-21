dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(Ultimarc-linux, 1.2)

AC_CANONICAL_HOST
AM_INIT_AUTOMAKE()

AC_PROG_CC
AM_PROG_CC_C_O

AC_CONFIG_MACRO_DIR([m4])
LT_INIT

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UINT16_T

# Checks for library functions.
AC_CHECK_FUNCS([memset strcasecmp strncasecmp strtol])

# Kernel headers path.
AC_ARG_WITH(kernel-headers,
    [AC_HELP_STRING([--with-kernel-headers=DIR],
        [specify path of Linux kernel headers [/usr/src/kernel-headers]])],
    [case "${withval}" in
        yes | no) AC_MSG_ERROR([bad value ${withval} for --with-kernel-headers]) ;;
        *)   KERNEL_HEADERS_DIR="${withval}" ;;
     esac],
    [KERNEL_HEADERS_DIR="/usr/src/kernel-headers"])

CPPFLAGS="-I$KERNEL_HEADERS_DIR/include"

AX_PKG_CHECK_MODULES([JSON], [], [json-c >= 0.11])
AX_PKG_CHECK_MODULES([LIBUSB], [], [libusb-1.0 >= 1.0.18])

AC_SUBST([ULTIMARC_LIB_VERSION], [1.2])

AC_CONFIG_FILES([Makefile
		 src/libs/Makefile
		 src/umtool/Makefile
		 ultimarc-${ULTIMARC_LIB_VERSION}.pc:ultimarc.pc.in])
AC_OUTPUT

