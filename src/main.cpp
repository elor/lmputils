#include <iostream>

#include <mpi.h>
#include <lammps.h>
#include <atom.h>
#include <input.h>
#include <library.h>

#include <read_data.h>

using namespace std;
using namespace LAMMPS_NS;

int main(int argc, char **argv)
{
  LAMMPS lammps(0, NULL, MPI_COMM_WORLD);

  //  lammps.input->one("atom_style charge");
  char style[]="charge";
  lammps.atom->create_avec("charge", 0, NULL, lammps.suffix);
  //  lammps.input->one("read_data Silane.lmp");

  ReadData rd(&lammps);
  char arg[] = "Silane.lmp";
  char *argp = arg;
  rd.command(1, &argp);

  cout << lammps.atom->natoms << " Atoms loaded" << endl;

  return 0;
}
