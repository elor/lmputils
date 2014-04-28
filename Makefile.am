AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -Iinclude

lib_LTLIBRARIES = liblmp2atomstyle.la liblmpio.la
include_HEADERS = include/lmpio.h include/lmp2atomstyle.h
liblmp2atomstyle_la_SOURCES = src/lmp2atomstyle.c
liblmp2atomstyle_la_LDFLAGS = -version-info 1:0:0
liblmpio_la_SOURCES = src/lmpio.cpp
liblmpio_la_LDFLAGS = -version-info 1:0:0 -llmp2atomstyle

bin_PROGRAMS = lmp2atomstyle
lmp2atomstyle_SOURCES = src/lmp2atomstyle_main.c
lmp2atomstyle_LDFLAGS = -llmp2atomstyle

check_PROGRAMS = lmpiotest
lmpiotest_SOURCES = src/lmpiotest.cpp
lmpiotest_LDADD = -llammps_custom -lmpi_stubs
lmpiotest_LDFLAGS = -lfftw3 -llmpio

