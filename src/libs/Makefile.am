# src/libs/Makefile.am

AM_CFLAGS = $(shell pkg-config --cflags json-c)
AM_LDFLAGS = $(shell pkg-config --libs json-c)

lib_LTLIBRARIES = libultimarc-0.1.la
libultimarc_0_1_la_SOURCES = ultimarc.c

otherincludedir = $(includedir)/ultimarc
otherinclude_HEADERS = ultimarc.h

