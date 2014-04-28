#!/bin/bash
#
# creates final.xyz, converts it to lmp data file, corrects it and invokes
# lammps for rdf calculation
# 
# syntax: $0 <dump.xyz>
# note: use "--" instead of a file name to read from stdin

# list of required software
required="python atomsk perl"

# show syntax and exit
fail(){
  echo "syntax: $0 <dump.xyz>" >&2
  echo "use '--' instead of a file name to read from stdin" >&2
  exit 1
}

# get the mass of an element
getmass(){
  python -c "import periodictable; print periodictable.$1._mass" && return
  echo "Error: Element '$1'not in periodic table" >&2
  echo "(or python error: import periodictable failed)" >&2
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
types=`sed -n '3,/^\s*[0-9]*\s*$/ s/^\s*\([A-Z][a-z]*\)\s.*$/\1/p' "$xyzfile" | perl -ne 'if (!defined $x{$_}) { print $_; $x{$_} = 1; }' | xargs`

rm "$lmpfile" 2>/dev/null
atomsk $xyzfile "$lmpfile" >/dev/null

# cleanup of temporary files
[ "$xyzisintmp" == "yes" ] && rm $xyzfile

# add zero charge to every atom if required (might work with reaxff only)
if [ "$(echo $atomstyle)" == "atom_style charge" ]; then
  sed -i 's/\(\s*[0-9][0-9]*\s\s*[1-9][0-9]*\s\)\(\(\s*[-0-9.+e]*\)\{3\}\s*\)$/\1 0.0 \2 /' "$lmpfile"
fi

# add mass information to lammps data file
masslines=$(
  i=0
  echo
  echo "Masses"
  echo
  for type in $types; do
    let i+=1
    mass=`getmass $type`
    echo "    $i    $mass"
  done
  echo
)

masslines="$(echo -e "$masslines" | sed 's/$/\\/')"

sed "/^\s*Atoms/i $masslines
" "$lmpfile"

rm "$lmpfile"

