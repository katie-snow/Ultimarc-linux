dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(Ultimarc-linux, 0.1)


AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE()

AC_CONFIG_HEADERS([src/libs/libultimarc.h])

AC_PROG_CC
AM_PROG_CC_C_O

LT_INIT



AC_SUBST([ULTIMARC_LIB_VERSION], [0.1])

#AC_CONFIG_FILES(Makefile src/libs/Makefile)
AC_CONFIG_FILES([Makefile 
				src/libs/Makefile
				src/umtool/Makefile				
				ultimarc-${ULTIMARC_LIB_VERSION}.pc:ultimarc.pc.in])
AC_OUTPUT

