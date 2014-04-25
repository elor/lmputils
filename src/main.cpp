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
  double *sizes = NULL;
  char *atom_style = NULL;

  lmpio_read(argv[1], &positions, &types, &masses, &sizes, &atom_style);

  if (positions)
    free(positions);
  if (masses)
    free(masses);
  if (types)
    free(types);
  if (sizes)
    free(sizes);
  if (atom_style)
    free(atom_style);

  return 0;
}
