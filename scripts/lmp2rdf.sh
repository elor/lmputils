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
required="lammps mass2element.py lmpcharges.sh lmp2atomstyle"

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

# get atom style by guessing from lmp file format
atomstyle="atom_style `lmp2atomstyle "$lmpfile" | grep -o '^[a-z]*'`"

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
$(
  if (( ${#types_arr[@]} > 1 )); then
    echo "compute rdfALL all rdf 400"
    echo "fix ALL all ave/time 10 1 10 c_rdfALL file all.rdf mode vector"
  fi
)
$(
  IFS=$'\r\n'
  for pair in $typepairs; do
    types=`echo $pair | cut -d: -f 1`
    ids=`echo $pair | cut -d: -f 2`
    echo "compute rdf$types all rdf 5000 $ids"
    echo "fix $types all ave/time 10 1 10 c_rdf$types file $types.rdf mode vector"
  done
)
EOF
)

echo "running lammps"

lammpscmds="
$atomstyle
read_data $lmpfile
velocity all zero linear
pair_style lj/cut 10
pair_coeff * * 0 0
$rdffixes
run 1
"

#echo -e "$lammpscmds"

# lammps run
lammps -nocite -echo screen -log none -suffix opt << EOF
$lammpscmds
EOF

(( $? != 0 )) && echo "lammps failed!" || echo "rdf calculations done"
