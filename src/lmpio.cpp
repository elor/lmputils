#include <lmpio.h>

#include <lmp2atomstyle.h>

#include <iostream>
#include <cassert>
#include <cstring>
#include <vector>
#include <sstream>
#include <iterator>

#include <mpi.h>
#include <lammps.h>
#include <atom.h>
#include <input.h>
#include <domain.h>
#include <read_data.h>

using namespace std;
using namespace LAMMPS_NS;

#ifndef SMALL_BUFFER_LENGTH
#define SMALL_BUFFER_LENGTH 128
#endif

extern int lmpio_read(const char *filename, double **positions, int **types, double **masses, double **size, char **atom_style, int *numatoms, int *numtypes) {

  if (numtypes == NULL){
    if (masses != NULL) {
      return 1;
    }
  }

  if (numatoms == NULL) {
    if (positions != NULL || types !=NULL) {
      return 2;
    }
  }

  // let lmp2atom
  lmphandle lmp = lmp2atomstyle_create();
  if (lmp2atomstyle_parse_file(lmp, filename) != 0) {
    return 3;
  }

  // read the style directly into the lammps command
  char style[SMALL_BUFFER_LENGTH];
  int ret = lmp2atomstyle_get_style(lmp, style, SMALL_BUFFER_LENGTH);

  free(lmp);

  if (ret != 0) {
    cerr << "cannot read atom_style from lmp file " << filename << endl;
    return 4;
  }

  // ignore all alternative styles and use the first one
  char *ptr = style + strcspn(style, "/ \t\n\r");
  if (ptr != NULL)
    ptr[0] = '\0';

  // now, run lammps to read the file

  // LAMMPS options
  // "-screen" won't be set, because when lammps calls fail, they will call exit(1)
  // this way, the user can at least see where LAMMPS failed
  // obvious drawback: the user sees all unfiltered lammps output
  string opts = "ald -log none -echo none -suffix opt -nocite";

  vector<string> tokens;
  istringstream iss(opts);
  copy(istream_iterator<string>(iss),
       istream_iterator<string>(),
       back_inserter<vector<string> >(tokens)
       );

  char ** argv = new char*[tokens.size()];

  for (int i = 0; i < tokens.size(); ++i) {
    argv[i] = strdup(tokens[i].c_str());
  }

  int argc = tokens.size();

  LAMMPS lammps(argc, argv, MPI_COMM_WORLD);
  for (int i = 0; i < tokens.size(); ++i)
    {
      free(argv[i]);
    }

  delete[] argv;

  // atom_style
  lammps.atom->create_avec(style, 0, NULL, lammps.suffix);

  // read_data
  ReadData rd(&lammps);
  char *filename_nonconst = strdup(filename);
  // this command can fail dramatically
  // it will then call exit(1)
  rd.command(1, &filename_nonconst);
  free(filename_nonconst);

  //  cout << lammps.atom->natoms << " Atoms loaded" << endl;

  if (lammps.atom->natoms == 0) {
    return 5;
  }

  assert(lammps.atom->tag_enable != 0);
  assert(lammps.atom->tag_consecutive() != 0);

  if (lammps.atom->natoms > MAXSMALLINT) {
    return 6;
  }

  // extract number of atoms
  if (numatoms != NULL) {
    *numatoms = lammps.atom->natoms;
    assert(*numatoms == lammps.atom->nlocal);
  }

  // extract number of types
  if (numtypes != NULL) {
    // TODO verify
    *numtypes = lammps.atom->ntypes;
  }

  // extract atoms
  if (positions != NULL) {
    double **x = lammps.atom->x;
    int *tag = lammps.atom->tag;

    // @user: don't forget to free()
    *positions = reinterpret_cast<double*>(malloc((*numatoms) * sizeof(double) * 3));

    // copy flattened atom positions
    size_t id, idoffset;
    for (size_t atom = 0; atom < *numatoms; ++atom, ++x) {
      id = tag[atom] - 1;

      idoffset = id * 3;
      for (size_t coord = 0; coord < 3; ++coord) {
        (*positions)[idoffset + coord] = (*x)[coord];
      }
    }

    // done
  }

  // extract types
  if (types != NULL) {
    int *t = lammps.atom->type;
    int *tag = lammps.atom->tag;

    //@user: don't forget to free()
    *types = reinterpret_cast<int*>(malloc((*numatoms) * sizeof(int)));

    size_t id;
    for (size_t atom = 0; atom < *numatoms; ++atom, ++t) {
      id = tag[atom] - 1;

      (*types)[id] = *t;
    }

    //done
  }

  // extract masses
  if (masses != NULL) {
    double *mass = lammps.atom->mass;

    *masses = reinterpret_cast<double*>(malloc((*numtypes) * sizeof(double)));
    
    for (size_t type = 0; type < *numtypes; ++type) {
      (*masses)[type] = mass[type+1];
    }

    // done
  }

  // extract size
  if (size != NULL) {
    assert(lammps.domain->box_exist == 1);
    assert(lammps.domain->dimension == 3);
    
    if (lammps.domain->triclinic != 0) {
      return 7;
    }

    // don't forget to free()
    *size = reinterpret_cast<double*>(malloc(6 * sizeof(double)));

    (*size)[0] = lammps.domain->boxlo[0];
    (*size)[1] = lammps.domain->boxhi[0];
    (*size)[2] = lammps.domain->boxlo[1];
    (*size)[3] = lammps.domain->boxhi[1];
    (*size)[4] = lammps.domain->boxlo[2];
    (*size)[5] = lammps.domain->boxhi[2];

    // done
  }

  return 0;
}
  
extern int lmpio_write(const char *filename, int numatoms, int numtypes, double *positions, int *types, double *masses, double *size, char **atom_style) {
  cerr << "lmpio_write not implemented yet" << endl;
  return -1;
}
