#!/usr/bin/env python
#
# returns the combined mass of all atoms in mass units (usually atomic units)

import sys, re

if len(sys.argv) != 2:
  sys.stderr.write("Syntax: %s <file.lmp>\n")
  sys.exit(1)

filename = sys.argv[1]

# read the original size into an object
if filename != '--':
  inputfile=open(filename)
else:
  inputfile=sys.stdin

mass = 0.0
masses={}

section=''

for line in inputfile:
  # skip empty lines
  line = line.strip()
  if len(line) == 0:
    continue
  if line[0] == '#':
    continue
  if re.match('^[A-Z][a-z]*$', line):
    section=line.lower()
    continue

  if section == 'atoms':
    # get types and add their masses up
    type=int(line.split()[1])
    mass += masses[type-1]
  elif section == 'masses':
    type,typemass=line.split()
    type=int(type)
    typemass=float(typemass)

    masses[type-1]=typemass
  else:
    # wait until we're in the right section
    continue

print mass

