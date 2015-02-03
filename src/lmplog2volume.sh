#!/bin/bash
#
# reads the last thermo line from a .lmp file with npt and prints its last value
# usually, this is the volume

cat "$1" | sed -n '/^Memory usage/,/^Loop time/p' | sed '/^[^09 ]/d' | tail -n1 | xargs -n1 | tail -n1

