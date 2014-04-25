#ifndef _LMPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

  /**
   * Read a file and allocate new arrays which will be returned using the doublepointer variables
   *
   * if you set an output argument to NULL, it will be ignored
   *
   * @return number of atoms on success, 0 on format error, -1 on internal failure
   */
  extern int lmpio_read(const char *filename, double **positions, int **types, double **masses, double **size, char **atom_style);
  
  extern int lmpio_write(const char *filename, int numatoms, double *positions, int *types, double *masses, double *size, char **atom_style);
  
#ifdef __cplusplus
}
#endif

#endif // _LMPIO_H_
