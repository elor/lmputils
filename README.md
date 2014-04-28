# lmputils

## Description

lmputils provide a set of helper programs for working with lammps data files.

Note: Most scripts are tested with the atom styles _atomic_ and _charge_ only.

## Installation

    ./configure
    make
    make install

## Contents

### lmp2atomstyle

Both a program and a library which guesses the required atom style for a file

TODO add usage section

### lmpio

The lmpio library provides a few functions to read from and (eventually) write
to lmp files.

TODO add usage section

### lmp2rdf.sh

Extracts rdf data from a *.lmp and a lammps.log file. Requires a lammps binary

### lmpcharges.sh

*Deprecated. Use lmpsizes.sh to add charges*

Converts an _atomic_ atom style to a _charge_ atom style

### lmpdensity.sh

Prints the density of the given *.lmp file

### lmpmass.py

Prints the mass of the given *.lmp file

### lmpsizes.py

Manipulates the box size of a *.lmp file.

See `lmpsizes.py --help` for a list of available commands

### lmpvolume.py

Prints the volume of a *.lmp file

### xyz2lmp.sh

Converts a *.xyz file to *.lmp format.


