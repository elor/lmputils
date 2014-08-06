#!/bin/bash
#
# writes mass, volume and density of all arguments (lmp files) to stdout

helpandexit(){
    cat <<EOF >&2
Usage: $0 <file.lmp>...
EOF
    exit 1
}

(( ${#@} > 0)) || helpandexit

echo -e '#filename\tmass/u\tvolume/A^3\tdensity/g/cm^3'
for file; do
    echo -e "$file\t`lmpmass.py "$file"`\t`lmpvolume.py "$file"`\t`lmpdensity.sh "$file"`"
done
