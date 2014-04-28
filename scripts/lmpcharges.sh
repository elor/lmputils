#!/bin/bash
#
# syntax: $0 <dump.xyz> <log.lammps>

# list of required software
required="lmpsizes.sh"

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

lmpsizes.py "$lmpfile"

