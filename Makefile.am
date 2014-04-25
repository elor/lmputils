AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -Iinclude #-fopenmp

lib_LTLIBRARIES = liblmpio.la
liblmpio_la_SOURCES = src/lmpio.cpp
liblmpio_la_LDFLAGS = -version-info 1:0:0
include_HEADERS = include/lmpio.h

bin_PROGRAMS = lmpio
lmpio_SOURCES = src/main.cpp
lmpio_LDADD = -llammps_custom -lmpi_stubs
lmpio_LDFLAGS = -lfftw3 -lenskmc -llmp2atomstyle -llammps_custom -llmpio

#bin_SCRIPTS = src/*.sh

