#!/bin/bash
#
# creates final.xyz, converts it to lmp data file, corrects it and invokes
# lammps for rdf calculation
# 
# syntax: $0 <dump.xyz>
# note: use "--" instead of a file name to read from stdin

# list of required software
required="python atomsk perl lmpsizes.py lmpsetmasses.sh"

# show syntax and exit
fail(){
  echo "syntax: $0 <dump.xyz>" >&2
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
xyzfile="$1"
if [ "$xyzfile" == "--" ] ; then
  xyzfile=/tmp/pid$$_$RANDOM.xyz
  xyzisintmp="yes"
  cat > $xyzfile
fi
[ -f "$xyzfile" ] || [ -z "$xyzfile" ] || fail
lmpfile=/tmp/pid$$_$RANDOM.lmp

# retrieve unique types in order
types=`sed -n '3,/^\s*[0-9]*\s*$/ s/^\s*\([A-Z][a-z]*\)\s.*$/\1/p' < "$xyzfile" | perl -ne 'if (!defined $x{$_}) { print $_; $x{$_} = 1; }' | xargs`

# actual work
atomsk $xyzfile $lmpfile >/dev/null || exit 1
mv $lmpfile{,.raw}
lmpsizes.py $lmpfile.raw s > $lmpfile || exit 1
rm $lmpfile.raw
lmpsetmasses.sh "$types" $lmpfile

# cleanup of temporary files
[ "$xyzisintmp" == "yes" ] && rm $xyzfile

cat $lmpfile
rm $lmpfile

