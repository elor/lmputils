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
required="lammps python atomsk perl mass2element.py"

# show syntax and exit
fail(){
  echo "syntax: $0 <data.lmp> <log.lammps>" >&2
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
(( ${#@} == 2 )) || fail
lmpfile="$1"
[ -f "$lmpfile" ] || fail
logfile="$2"
[ -f "$logfile" ] || fail

# prepare directories
mkdir -p rdf

# retrieve LAMMPS parameters
atomstyle=`grep atom_style "$logfile"`
pairstyle=`grep -e '^\\s*pair_\\|fix qeq' "$logfile" | grep -v '#' | grep -v '\\$'`

# retrieve masses in order
masses=`echo $(sed -n '/Masses/,/Atoms/ s/^\s*[1-9][0-9]*\s*//p' "$lmpfile")`

# get types from masses
types=$(
  for mass in $masses; do
    echo $(mass2element.py $mass) | cut -d ' ' -f 1
  done
)

# concatenate types to unique pairs
types_arr=($types)
typepairs=$(
  for i in `seq 1 ${#types_arr[@]}`; do
    let tmpi=i-1
    for j in `seq $i ${#types_arr[@]}`; do
      let tmpj=j-1
      echo "${types_arr[$tmpi]}${types_arr[$tmpj]}:$i $j"
    done
  done
)

# create lammps fixes
rdffixes=$(cat <<EOF
compute rdfALL all rdf 400
fix ALL all ave/time 10 1 10 c_rdfALL file rdf/all.rdf mode vector
$(
  IFS=$'\r\n'
  for pair in $typepairs; do
    types=`echo $pair | cut -d: -f 1`
    ids=`echo $pair | cut -d: -f 2`
    echo "compute rdf$types all rdf 400 $ids"
    echo "fix $types all ave/time 10 1 10 c_rdf$types file rdf/$types.rdf mode vector"
  done
)
EOF
)

echo "running lammps"

# lammps run
lammps -nocite -log none -echo screen -screen none -suffix opt << EOF
$atomstyle
read_data $lmpfile
$pairstyle
$rdffixes
run 1
EOF

echo 
echo "rdf calculations done"

