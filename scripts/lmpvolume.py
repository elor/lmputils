#!/usr/bin/env python

import sys, re

if len(sys.argv) != 2:
  print("Syntax: %s <file.lmp>")
  sys.exit(1)

filename = sys.argv[1]

# read the original size into an object
if filename != '--':
  infile=open(filename)
else:
  infile=sys.stdin

lines=[ line.split() for line in infile if re.match('^(\s*[-+.0-9eE]+\s){2}\s*[xyz]lo\s+[xyz]hi\s*$', line) ]

if infile != sys.stdin:
  infile.close()

borders={}

for line in lines:
  if len(line) != 4:
    continue
  borders[line[2]] = float(line[0])
  borders[line[3]] = float(line[1])

volume = 1.0
volume *= borders['xhi']-borders['xlo']
volume *= borders['yhi']-borders['ylo']
if borders.has_key('zhi') and borders.has_key('zlo'):
  volume *= borders['zhi']-borders['zlo']

print volume

