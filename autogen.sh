#!/bin/sh
#
# prepare build environment

mkdir -p m4
autoreconf --force --install -I config -I m4
