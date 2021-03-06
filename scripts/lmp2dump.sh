#!/bin/bash
#
# uses lammps to convert a .lmp file to a LAMMPS dump file
# 
# syntax: $0 <dump.xyz> <log.lammps>
#
# TODO: PBS options
# TODO: module load

# list of required software
required="lammps lmpcharges.sh"

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
  echo >&2
  fail
fi

# validate input
(( ${#@} == 1 )) || fail
lmpfile="$1"
[ -f "$lmpfile" ] || fail

# preparing lmp copy
tmpfile=/tmp/pid$$_$RANDOM.lmp
lmpcharges.sh "$lmpfile" > $tmpfile
tmpfile2=`sed 's/\.lmp$/.dump/' <<< $tmpfile`

[ $tmpfile == $tmpfile2 ] && echo "internal error: tmpfile == tmpfile2" >&2 && exit

echo "running lammps" >&2

numtypes="`sed -n 's/^\s*\([0-9]\+\)\s\+atom\stypes\s*$/\1/p' $tmpfile`"

(( $numtypes > 0 )) || echo "cannot read number of atom types from lmp file" >&2

setmasses="$(for f in `seq 1 $numtypes`; do
	echo "mass $f 1.0"
done)"

lammpscmds="
units real
atom_style charge
atom_modify sort 1 2.0
read_data $tmpfile
$setmasses
pair_style none
timestep 1
dump 0 all atom 2 $tmpfile2
fix 1 all nve
run 1
"

#echo -e "$lammpscmds" >&2

# lammps run
lammps -nocite -echo screen -log none -suffix opt >&2 << EOF
$lammpscmds
EOF

echo >&2

(( $? != 0 )) && echo "lammps failed!" >&2 || echo "dump conversion finished" >&2

if [ "$tmpfile" != "$lmpfile" ]; then
  rm $tmpfile
fi

cat $tmpfile2
rm $tmpfile2

