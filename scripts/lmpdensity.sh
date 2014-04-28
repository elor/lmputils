#!/bin/bash
#
# creates final.xyz, converts it to lmp data file, corrects it and invokes
# lammps for rdf calculation
# 
# syntax: $0 <dump.xyz>
# note: use "--" instead of a file name to read from stdin

# list of required software
required="python lmpvolume.py lmpmass.py"

# show syntax and exit
fail(){
  echo "syntax: lmpdensity.sh <file.lmp>" >&2
  echo "use '--' instead of a file name to read from stdin" >&2
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
if [ "$lmpfile" == "--" ] ; then
  lmpfile=/tmp/pid$$_$RANDOM.xyz
  removetmp="rm $lmpfile"
  cat > $lmpfile
fi
[ -z "$lmpfile" ] || [ -s "$lmpfile" ] || fail

volume=`lmpvolume.py $lmpfile`
mass=`lmpmass.py $lmpfile`

python -c "print $mass * 1.6605389182748003 / $volume"

$removetmp

