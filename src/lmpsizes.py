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

    +[number]
        remove padding of [number] units from the box

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
        * shrink-wrap (with padding): "s[number]"
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

def getVolume(sizes):
  volume = 1.0;
  volume *= sizes['xhi']-sizes['xlo']
  volume *= sizes['yhi']-sizes['ylo']
  if sizes.has_key('zhi') and sizes.has_key('zlo'):
    volume *= sizes['zhi']-sizes['zlo']
  return volume

def operationSetVolume(data, volume):
  current = getVolume(data['sizes'])
  factor = (volume / current)**(1.0/3)
  return operationScale(data, factor)

def operationScale(data, factor):
  rescaled = {}
  sizes=data['sizes']
  for key in sizes.keys():
    rescaled[key] = sizes[key]*factor
  data['sizes'] = rescaled
  return data

def operationAddPadding(data, padding):
  padded = data['sizes'].copy()
  for key in padded.keys():
    if re.match('^.hi$', key):
      padded[key] += padding
    elif re.match('^.lo$', key):
      padded[key] -= padding
  data['sizes'] = padded
  return data

def isNumber(string):
  try:
    float(string)
    return True
  except ValueError:
    return False

def chainOperation(string, operation):
  code = string[0]
  rest = string[1:]
  if code == '*':
    value = float(rest)
    return lambda x: operationScale(operation(x), value)
  elif code == '+':
    value = float(rest)
    return lambda x: operationAddPadding(operation(x), value)
  elif code == '-':
    value = float(string)
    return lambda x: operationAddPadding(operation(x), value)
  elif code == '=':
    value = float(rest)
    return lambda x: operationSetVolume(operation(x), value)
  elif code == '-':
    raise RuntimeError("unknown operation", string)
  elif isNumber(string):
    return chainOperation('*%s'%string, operation)
  else:
    raise RuntimeError("unknown operation", string)

def lineToAtom(line):
  atom = {
    'id': 0,
    'type': 0,
    'q': 0.0,
    'x': 0.0,
    'y': 0.0,
    'z': 0.0,
    'nx': 0,
    'ny': 0,
    'nz':0,
  }
  if len(line) == 5:
    # id type x y z
    atom['id'] = int(line[0])
    atom['type'] = int(line[1])
    atom['x'] = float(line[2])
    atom['y'] = float(line[3])
    atom['z'] = float(line[4])
  elif len(line) == 6:
    # id type q x y z
    atom['id'] = int(line[0])
    atom['type'] = int(line[1])
    atom['q'] = float(line[2])
    atom['x'] = float(line[3])
    atom['y'] = float(line[4])
    atom['z'] = float(line[5])
  elif len(line) == 8:
    # id type x y z nx ny nz
    atom['id'] = int(line[0])
    atom['type'] = int(line[1])
    atom['x'] = float(line[2])
    atom['y'] = float(line[3])
    atom['z'] = float(line[4])
    atom['nx'] = int(line[5])
    atom['ny'] = int(line[6])
    atom['nz'] = int(line[7])
  elif len(line) == 9:
    # id type q x y z nx ny nz
    atom['id'] = int(line[0])
    atom['type'] = int(line[1])
    atom['q'] = float(line[2])
    atom['x'] = float(line[3])
    atom['y'] = float(line[4])
    atom['z'] = float(line[5])
    atom['nx'] = int(line[6])
    atom['ny'] = int(line[7])
    atom['nz'] = int(line[8])
  else:
    raise RuntimeError('atom line: invalid number of arguments: ', line)

  return atom

def atomToLine(atom):
  return "%6d %3d %9.5f %20.13f %20.13f %20.13f %2d %2d %2d"%(atom['id'], atom['type'], atom['q'], atom['x'], atom['y'], atom['z'], atom['nx'], atom['ny'], atom['nz']);

def parseSizes(lines):
  sizes={}

  sizelines=[ line.split() for line in lines if re.match('^([-+.0-9eE]+\s+){2}[xyz]lo\s+[xyz]hi$', line.strip()) ]

  for line in sizelines:
    if len(line) != 4:
      continue
    sizes[line[2]] = float(line[0])
    sizes[line[3]] = float(line[1])

  return sizes

stdin=None

def readFile(filename):
  global stdin

  if filename == '--':
    if stdin:
      return stdin[:]
    else:
      stdin = [ line for line in sys.stdin ]
      return stdin[:]
  else:
    try:
      thefile=open(basisname)
    except IOError:
      sys.stderr.write("Cannot open file '%s'\n\n"%basisname)
      sys.exit(1)

    lines = [ line for line in thefile ]
    thefile.close()

    return lines

def parseAtoms(lines):
  atomindex=[line.strip() for line in lines].index('Atoms')

  atoms=[]
  for line in lines[atomindex+1:]:
    if re.match('^\s*$', line):
      continue
    if re.match('^\s*[+-.0-9eE \t]+\s*$', line):
      atoms.append(lineToAtom(line.split()))
    else:
      break;

  return atoms

def parseData(lines):
  data = {
    sizes: parseSizes(lines),
    atoms: parseAtoms(lines),
  }

  return data

def printAtoms(atoms):
  print "Atoms"
  print
  for atom in atoms:
    print atomToLine(atom)
  print

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

# daisychain operations
operation = lambda x: x
for i in range(3, len(sys.argv)):
  operation = chainOperation(sys.argv[i], operation)

lines = readFile(srcname)

# read sizes from basisfile
sizes=parseSizes(readFile(basisname))
# read atoms from srcfile
atoms = parseAtoms(lines)

data = {
  'sizes' : sizes,
  'atoms' : atoms,
}

# apply all operations
data = operation(data)

atoms = data['atoms']
sizes = data['sizes']

section=''

# insert into old file and print
for line in lines:
  if line == lines[0]:
    # ignore the comment line
    continue

  if re.match('^[A-Z][a-zA-Z ]*$', line):
    section = ' '.join(line.split())

  if section == '' and re.match('^(\s*[-+.0-9eE]+\s){2}\s*[xyz]lo\s+[xyz]hi\s*$', line):
    line = line.split()
    print("      %f      %f  %s %s"%(sizes[line[2]], sizes[line[3]], line[2], line[3]))
  elif section == 'Atoms':
    if re.match('^\s*Atoms\s*$', line):
      # print all preformatted atoms
      printAtoms(atoms)
    else:
      # don't print the normal atoms and empty lines
      pass
  else:
    print(line),

