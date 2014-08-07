#!/bin/bash
#
# sets the masses in lmp files to the specified elements'

helpexit(){
    cat <<EOF >&2

Syntax: $0 'elem1 elem2' <file.lmp>...
EOF

#To read from stdin, use '--' as a file name (works only once)

    exit 1
}

(( ${#@} >= 2 )) || { echo "Not enough arguments">&2; helpexit; }
which element2mass.py&>/dev/null || { echo "element2mass.py not found in PATH">&2; helpexit; }

# global variable, used in addmasses
elements="$1"
shift

validatefile(){
#    [ "$1" == "--" ] && return 0
    [ -f "$1" ] || { echo "file '$1' doesn't exist">&2; return 1; }
    [ -s "$1" ] || { echo "file '$1' is empty">&2; return 1; }
    return 0
}

removemasses(){
#    [ "$1" == '--' ] && local opt="" || local opt="-i"
    opt=-i

    sed $opt '/^Masses\s*$/,/^Atoms\s*$/ { /^Atoms\s*/b;d }' "$1"
}

massestmpfile(){
    tmpfile=/tmp/masses-$$-$RANDOM.tmp

    {
        echo "Masses"
        echo
        i=1
        for elem; do
            echo -e "       $i     `element2mass.py $elem`"
            let i+=1
        done
        echo
        echo "Atoms"
    } > $tmpfile

    echo $tmpfile
}

tmpfile=`massestmpfile $elements`

writemasses(){
#    [ "$1" == '--' ] && local opt="" || local opt="-i"
    opt=-i

    sed $opt "/^Atoms\s*$/ {\
r $tmpfile
d
}" "$1"
}

for file; do
    removemasses "$file"
    writemasses "$file"
done

rm $tmpfile
