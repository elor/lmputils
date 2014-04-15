#!/usr/bin/env python

import sys, re

# init 

def printHelp():
  sys.stderr.write("""\
lmpsizes.sh, a tool for adjusting the size of a .lmp file
Syntax: lmpsizes.sh <source.lmp> <basis.lmp> (operation)...
        lmpsizes.sh -h|--help

Description:
    This script reads the size limits from basis.lmp, alters them using any number of operations and replaces the size limits of source.lmp. Results are printed to STDOUT. To read from STDIN, simply write '--' instead of a file name.
    NOTE: Atom positions aren't modified or validated.
    NOTE: From <basis.lmp>, only the "[num] [num] [num] ?hi ?lo" lines are read, the rest is discarded

Options:
    -h, --help        show this page

Operations:
    You can apply any number of operations to the read size

    [number]
        alias for *[number]

    *[number]
        linearly scale every value by [number]

    +[number]
        add a padding of [number] units to the box

    =[number]
        set the volume of the box to [number] units^3

Examples:
    Here are some examples of how to use lmpsizes.sh:
    
    Set the size of source.lmp to the size of basis.lmp

            lmpsizes.sh source.lmp basis.lmp

    Double the size of source.lmp (without moving any atoms):

            lmpsizes.sh source.lmp source.lmp *2.0
        or (fallback)
            lmpsizes.sh source.lmp source.lmp 2.0

    Add 5.0 units padding

            lmpsizes.sh source.lmp source.lmp +5.0

    Read from stdin

            cat source.lmp | lmpsizes.sh -- basis.lmp
            cat source.lmp | lmpsizes.sh source.lmp --
            cat source.lmp | lmpsizes.sh -- --

    Scale to a certain volume
    
            lmpsizes.sh source.lmp basis.lmp =7000
    
    Write to file.lmp

            lmpsizes.sh source.lmp basis.lmp > file.lmp

    Apply multiple operations
    
            lmpsizes source.lmp basis.lmp =7000.0 +5.0 *0.95

TODO

    Some time in the future, the following features may be added.
    DISCLAIMER: There's no guarantee when or if a feature will be added.

        * Output to a predefined file: "-o <output.lmp>"
        * Validation of input ranges (non-negative scaling, etc.)
        * Validation of atom positions ( >= atom positions)
        * shrink-wrap to contained atoms
        * modify atom positions
        * periodic images of atoms
        * read sizes from a parameter instead of basis.lmp

Author

    lmpsizes.sh was written by Erik E. Lorenz <erik.e.lorenz@gmail.com>
  """)

def suggestHelp():
  sys.stderr.write("""\
Syntax: lmpsizes.sh <source.lmp> <basis.lmp> (operation)...

See lmpsizes.sh --help for detailed information.
  """)

def getVolume(borders):
  volume = 1.0;
  volume *= borders['xhi']-borders['xlo']
  volume *= borders['yhi']-borders['ylo']
  if borders.has_key('zhi') and borders.has_key('zlo'):
    volume *= borders['zhi']-borders['zlo']
  return volume

def operationSetVolume(borders, volume):
  current = getVolume(borders)
  factor = (volume / current)**(1.0/3)
  return operationScale(borders, factor)

def operationScale(operation, factor):
  rescaled = {}
  for key in borders.keys():
    rescaled[key] = borders[key]*factor
  return rescaled

def operationAddPadding(borders, padding):
  padded = borders.copy()
  for key in padded.keys():
    if re.match('^.hi$', key):
      padded[key] += padding
    elif re.match('^.lo$', key):
      padded[key] -= padding
  return padded

def isNumber(string):
  try:
    float(string)
    return True
  except ValueError:
    return False

def createOperation(string):
  code = string[0]
  rest = string[1:]
  if code == '*':
    value = float(rest)
    return lambda x: operationScale(x, value)
  elif code == '+':
    value = float(rest)
    return lambda x: operationAddPadding(x, value)
  elif code == '=':
    value = float(rest)
    return lambda x: operationSetVolume(x, value)
  elif code == '-':
    raise RuntimeError("unknown operation", string)
  elif isNumber(string):
    return createOperation('*%s'%string)
  else:
    raise RuntimeError("unknown operation", string)

try:
  sys.argv.index('-h')
  printHelp()
  sys.exit(1)
except ValueError:
  pass

try:
  sys.argv.index('--help')
  printHelp()
  sys.exit(1)
except ValueError:
  pass

if len(sys.argv) < 3:
  suggestHelp()
  sys.exit(1)

srcname = sys.argv[1]
basisname = sys.argv[2]

if basisname == '--' and srcname == '--':
  # buffer all of stdin for use as both files
  # I wish there was a better way
  stdin = [ line for line in sys.stdin ]
else:
  # don't buffer, but just read on the fly
  stdin = sys.stdin

# create list of operations
operations = [ createOperation(sys.argv[i]) for i in range(3, len(sys.argv)) ]

# read the original size into an object
if basisname != '--':
  try:
    atomfile=open(basisname)
  except IOError:
    sys.stderr.write("Cannot open file '%s'\n\n"%basisname)
    sys.exit(1)
else:
  atomfile=stdin

lines=[ line.split() for line in atomfile if re.match('^(\s*[-+.0-9eE]+\s){2}\s*[xyz]lo\s+[xyz]hi\s*$', line) ]

if atomfile != stdin:
  atomfile.close()

borders={}

for line in lines:
  if len(line) != 4:
    continue
  borders[line[2]] = float(line[0])
  borders[line[3]] = float(line[1])

# apply all operations
for op in operations:
  borders = op(borders)

# insert into old file and print
if srcname != '--':
  try:
    sizefile=open(srcname)
  except IOError:
    sys.stderr.write("Cannot open file '%s'\n\n"%srcname)
    sys.exit(1)
else:
  sizefile=stdin

for line in sizefile:
  if re.match('^(\s*[-+.0-9eE]+\s){2}\s*[xyz]lo\s+[xyz]hi\s*$', line):
    line = line.split()
    print("      %f      %f  %s %s"%(borders[line[2]], borders[line[3]], line[2], line[3]))
  else:
    print(line),

if sizefile != stdin:
  sizefile.close()

