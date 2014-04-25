AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -Iinclude #-fopenmp

lib_LTLIBRARIES = liblmp2atomstyle.la
liblmp2atomstyle_la_SOURCES = src/lmp2atomstyle.c
liblmp2atomstyle_la_LDFLAGS = -version-info 1:0:0
#otherincludedir = $(includedir)/THIS
#otherinclude_HEADERS = include/....h
#nobase_otherinclude_HEADERS = include/....h
include_HEADERS = include/lmp2atomstyle.h
#nobase_include_HEADERS = include/lmp2atomstyle.h

bin_PROGRAMS = lmp2atomstyle
lmp2atomstyle_SOURCES = src/lmp2atomstyle.c src/main.c
lmp2atomstyle_LDFLAGS = -lArgumentParser
lmp2atomstyle_CFLAGS = $(AM_CFLAGS)

#bin_SCRIPTS = src/*.sh

