#include <iostream>
#include <cstdlib>

#include <lmpio.h>

using namespace std;

int main(int argc, char **argv)
{
  if (argc != 2) {
    cerr << "invalid number of arguments: " << argc-1 << endl;
    return 1;
  }

  double *positions = NULL;
  double *masses = NULL;
  int *types = NULL;
  double *size = NULL;
  char *atom_style = NULL;
  int numtypes, numatoms;

  int ret = lmpio_read(argv[1], &positions, &types, &masses, &size, &atom_style, &numatoms, &numtypes);

  if (ret != 0) {
    cerr << "read error #" << ret << endl;
    return 1;
  }

  cout << "numatoms: " << numatoms << endl;
  cout << "numtypes: " << numtypes << endl;

  for (size_t i = 0; i < numatoms; ++i) {
    cout << (i+1) << " " << types[i] << " " << positions[3*i] << " " << positions[3*i+1] << " " << positions[3*i+2] << endl;
  }

  for (size_t j = 0; j < numtypes; ++j) {
    cout << (j+1) << " " << masses[j] << endl;
  }

  cout << "[ ";
  for (size_t k = 0; k < 6; ++k) {
    cout << size[k] << " ";
  }
  cout << "]" << endl;


  if (positions)
    free(positions);
  if (masses)
    free(masses);
  if (types)
    free(types);
  if (size)
    free(size);
  if (atom_style)
    free(atom_style);

  return 0;
}
