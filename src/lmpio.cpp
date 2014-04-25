#include <lmpio.h>

#include <lmp2atomstyle>

#include <iostream>

#include <mpi.h>
#include <lammps.h>
#include <atom.h>
#include <input.h>

using namespace std;
using namespace LAMMPS_NS;

#ifndef SMALL_BUFFER_LENGTH
#define SMALL_BUFFER_LENGTH 128
#endif

int lmpio_read(const char *filename, double **positions, int **types, double **masses, double **size, char **atom_style) {

  // let lmp2atom
  lmphandle lmp = lmp2atomstyle_create();
  if (lmp2atomstyle_parse_file(lmp, filename) != 0) {
    return -1;
  }

  // read the style directly into the lammps command
  char styleline[SMALL_BUFFER_LENGTH] = "atom_style ";
  char *style = strchr(styleline, '\0');
  int stylelen = SMALL_BUFFER_LENGTH - (style - styleline);
  if (lmp2atomstyle_get_style(lmp, style, stylelen) != 0) {
    return -1;
  }

  // ignore all alternative styles and try the first one
  char *ptr = style + strchr(style, '/ \t\n\r\0');
  ptr[0] = '\0';

  cout << styleline << endl;
}
  
int lmpio_write(const char *filename, int numatoms, double *positions, int *types, double *masses, double *size, char **atom_style) {
  cerr << "lmpio_write not implemented yet" << endl;
  return -1;
}
