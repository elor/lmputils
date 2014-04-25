#!/bin/sh
#
# prepare build environment

mkdir m4
autoreconf --force --install -I config -I m4
