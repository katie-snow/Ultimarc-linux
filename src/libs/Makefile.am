# src/libs/Makefile.am

AM_CFLAGS = $(JSON_CFLAGS) $(LIBUSB_CFLAGS)
AM_LDFLAGS = $(JSON_LIBS) $(LIBUSB_LIBS)


lib_LTLIBRARIES = libultimarc-0.1.la
libultimarc_0_1_la_SOURCES = ultimarc.c ipac.c

otherincludedir = $(includedir)/ultimarc
otherinclude_HEADERS = ultimarc.h ipac.h

