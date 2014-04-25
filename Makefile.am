AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -Iinclude #-fopenmp

#lib_LTLIBRARIES = libTHIS.la
#libArgumentParser_la_SOURCES = src/....cpp
#libArgumentParser_la_LDFLAGS = -version-info 0:0:0
#otherincludedir = $(includedir)/THIS
#otherinclude_HEADERS = include/....h
#nobase_otherinclude_HEADERS = include/....h
#include_HEADERS = include/THIS.h
#nobase_include_HEADERS = include/THIS.h

bin_PROGRAMS = lmpio
lmpio_SOURCES = src/main.cpp src/lmpio.cpp
lmpio_LDADD = -llammps_custom -lmpi_stubs
lmpio_LDFLAGS = -lfftw3 -lenskmc

#bin_SCRIPTS = src/*.sh

