


AM_CFLAGS = $(JSON_CFLAGS) $(LIBUSB_CFLAGS)
AM_LDFLAGS = $(JSON_LIBS) $(LIBUSB_LIBS)

bin_PROGRAMS=umtool
umtool_SOURCES=main.c
umtool_CPPFLAGS = -I$(top_srcdir)/src
umtool_LDADD = ../libs/libultimarc.la

