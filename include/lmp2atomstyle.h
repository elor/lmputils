#ifndef _LMP2ATOMSTYLE_H_

#include <stdlib.h>

#define lmphandle void*

#ifdef __cplusplus
extern "C" {
#endif

  /**
   * Creates a new lmphandle.
   * free() it when you're done.
   */
  lmphandle lmp2atomstyle_create(void);

  /**
   * Take a whole file and parse it
   * filenameis required to be null-terminated
   */
  extern int lmp2atomstyle_parse_file(lmphandle handle, const char *filename);

  /**
   * Take a buffer and parse it whole.
   * buffer is required to be null-terminated
   */
  extern int lmp2atomstyle_parse_buffer(lmphandle handle, const char *buffer);

  /**
   * take a line of a file and parse it
   * line is required to be null-terminated
   */
  extern int lmp2atomstyle_parse_line(lmphandle handle, const char *line);

  /**
   * check whether the lmphandle is ready to provide a style guess
   * 
   * @return 0 if ready, 1 if not, -1 on failure
   */
  extern int lmp2atomstyle_ready(lmphandle handle);

  /**
   * Retrieves the atom style and places it in out_style
   * 
   * @return 0 on success
   */
  extern int lmp2atomstyle_get_style(lmphandle handle, char *out_style, size_t buflen);

#ifdef __cplusplus
}
#endif

#endif //_LMP2ATOMSTYLE_H_
