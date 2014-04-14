#!/usr/bin/env python

import sys, re

if len(sys.argv) != 4 and len(sys.argv) != 3:
  print("Syntax: %s <source.lmp> <basis.lmp> (scalefactor)")
  sys.exit(1)

srcname = sys.argv[1]
basisname = sys.argv[2]
scalefactor = 1.0
if len(sys.argv) >= 4:
  scalefactor = float(sys.argv[3])

# read the original size into an object
if basisname != '--':
  infile=open(basisname)
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

# rescale the file
rescaled = { x: borders[x]*scalefactor for x in borders }

# insert into old file and print
if srcname != '--':
  outfile=open(srcname)
else:
  outfile=sys.stdin

for line in outfile:
  if re.match('^(\s*[-+.0-9eE]+\s){2}\s*[xyz]lo\s+[xyz]hi\s*$', line):
    line = line.split()
    print("      %f      %f  %s %s"%(rescaled[line[2]], rescaled[line[3]], line[2], line[3]))
  else:
    print(line),

if outfile != sys.stdin:
  outfile.close()

