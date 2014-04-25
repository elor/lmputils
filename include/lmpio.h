#ifndef _LMPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

  /**
   * Read a file and allocate new arrays which will be returned using the doublepointer variables
   *
   * if you set an output argument to NULL, it will be ignored
   *
   * @return 0 on success
   */
  extern int lmpio_read(const char *filename, double **positions, int **types, double **masses, double **size, char **atom_style, int *numatoms, int *numstyles);
  
  extern int lmpio_write(const char *filename, int numatoms, int numtypes, double *positions, int *types, double *masses, double *size, char **atom_style);
  
#ifdef __cplusplus
}
#endif

#endif // _LMPIO_H_
