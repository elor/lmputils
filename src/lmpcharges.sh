#!/bin/bash
#
# creates final.xyz, converts it to lmp data file, corrects it and invokes
# lammps for rdf calculation
# 
# syntax: $0 <dump.xyz> <log.lammps>
#
# TODO: PBS options
# TODO: module load

# list of required software
required=""

# show syntax and exit
fail(){
  echo "syntax: $0 <data.lmp>" >&2
  exit 1
}

# find and print missing programs
missing=$(
for prog in $required; do
  type $prog >/dev/null 2>/dev/null || echo $prog
done
)
if [ -n "$missing" ];then
  echo "This script requires the following libraries:" >&2
  echo -e "$missing" | sed 's/^/* /' >&2
  echo
  fail
fi

# validate input
(( ${#@} == 1 )) || fail
lmpfile="$1"
[ -f "$lmpfile" ] || fail

sed 's/\(\s*[0-9][0-9]*\s\s*[1-9][0-9]*\s\)\(\(\s*[-0-9.+e]*\)\{3\}\s*\)$/\1 0.0 \2 /' $lmpfile

