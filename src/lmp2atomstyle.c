#include <lmp2atomstyle.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define BUFFERSIZE 1024

struct lmp_t {
  uint16_t flags;
  uint8_t numprops;
};

#ifndef bool
#define bool int
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

enum {

  // headers
  headers=0xfff,
  atoms=0x1,
  bonds=0x2,
  angles=0x4,
  dihedrals=0x8,
  impropers=0x10,
  ellipsoids=0x20,
  lines=0x40,
  triangles=0x80,
  bodies=0x100,

  // sections
  sections=0x3000,
  nosection=0x0000,
  incomment=0x1000,
  inheaders=0x2000,
  inatoms=0x3000,
};

/**
 * Creates a new lmphandle.
 * free() it when you're done.
 */
extern lmphandle lmp2atomstyle_create(void) {
  struct lmp_t *handle = (struct lmp_t*)malloc(sizeof(struct lmp_t));
  handle->flags = incomment;
  handle->numprops = 0;

  return (lmphandle)handle;
}

/**
 * Take a whole file and parse it
 * filenameis required to be null-terminated
 */
extern int lmp2atomstyle_parse_file(lmphandle handle, const char *filename) {
  if (!handle) {
    return -1;
  }
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    return -1;
  }

  char *line = NULL;
  size_t len = 0;
  size_t read = 0;
  int ret;
  while (!feof(file)) {
    read = getline(&line, &len, file);
#ifdef DEBUG
    printf("read: ");
    printf(line);
#endif

    switch (read) {
    case -1:
      if (!feof(file)) {
        perror("lmp2atomstyle_parse_file: unexpected error\n");
        free(line);
        line = NULL;
        return -1;
      }
#ifdef DEBUG
      printf("eof\n");
#endif
      break;
    case 0:
      // ignore empty lines
#ifdef DEBUG
      printf("empty\n");
#endif
      break;
    default:
      ret = lmp2atomstyle_parse_line(handle, line);
#ifdef DEBUG
      printf("parsed. ret: %d\n", ret);
#endif
      // pass
      break;
    }

#ifdef DEBUG
    printf("line freed\n");
#endif

    free(line);
    line = NULL;
    if (ret != 0) {
      break;
    }
  }

  fclose(file);
  return 0;
}

/**
 * Take a buffer and parse it whole.
 * buffer is required to be null-terminated
 */
extern int lmp2atomstyle_parse_buffer(lmphandle handle, const char *buffer) {
  if (!handle) {
    return -1;
  }
  char linebuf[BUFFERSIZE];
  const char *start = buffer;
  const char *end = start;
  const char *endall = strchr(start, '\0');
  const char delim='\n';

  size_t linelength;
  int ret;
  while (end != NULL) {
    if (end == start) {
      // 
      ++start;
      end = strchr(start, delim);
      // continue;
    }

    linelength = end - start - 1;
    end = start;

    // skip empty lines
    if (linelength > 0) {
      if (linelength >= BUFFERSIZE) {
        sprintf(linebuf, "lmp2atomstyle_parse_buffer: BUFFERSIZE too low:\nBUFFERSIZE=%lu\nlinelength=%lu\n", BUFFERSIZE, linelength);
        fprintf(stderr, linebuf);
        end = start = NULL;
        break;
      }

      memcpy(linebuf, start, linelength);
      linebuf[linelength] = '\0';

      ret = lmp2atomstyle_parse_line(handle, linebuf);
      if (ret != 0) {
        fprintf(stderr, "lmp2atomstyle_parse_file: unexpected error");
        return -1;
      }
    }
  }

  // don't forget the last line  
  if (start != NULL && start[0] != '\0') {
    lmp2atomstyle_parse_line(handle, start);
    start = end;
  }

  return 0;
}

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

bool isSectionName(const char *name) {
  if (!isupper(name[0])) {
    return false;
  }

  size_t len = strlen(name);
  size_t i;
  for (i = 1; i < len; ++i) {
    if (!isalnum(name[i])) {
      if (isspace(name[i])) {
        // "'Coeffs' must follow"
        return (strcmp(&name[i+1], "Coeffs") == 0);
      }
      return false;
    }
  }

  return true;
}

size_t countwords(const char *line) {
  int words = 0;
  bool wasspace = true;

  const char *ptr = line;

  while (*ptr != '\0') {
    if (isspace(*ptr)) {
      wasspace = true;
    } else if (wasspace) {
      wasspace = false;
      ++words;
    }
    ++ptr;
  }

  return words;
}

/**
 * take a line of a file and parse it
 * line is required to be null-terminated
 */
extern int lmp2atomstyle_parse_line(lmphandle handle, const char *line) {
  if (!handle || line == NULL) {
    return -1;
  }
  struct lmp_t *lmp = (struct lmp_t*)handle;

  if ((lmp->flags & sections) == incomment) {
    // ignore the first line (comment) and wait for the headers
    lmp->flags &= ~sections;
    lmp->flags |= inheaders;
    return 0;
  }

  size_t len = strlen(line);
  if (len <= 0) {
    // skip empty lines
    return 0;
  }

  // strip trailing white spaces
  const char *ptr;
  ptr = line;
  while (true) {
    switch (*ptr) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      ++ptr;
      continue;
    }
    break;
  }

  if (*ptr == '\0') {
    // skip empty lines
    return 0;
  }
  if (*ptr == '#') {
    // skip comment lines
    return 0;
  }

  // check if this is a section
  if (ptr == line) {
    // is this the atoms section?
    char *buf = strdup(line);
    trimwhitespace(buf);

    if (isSectionName(buf)) {
      lmp->flags &= ~sections;

      if (strcmp("Atoms", buf) == 0) {
        lmp->flags |= inatoms;
      } else {
        // nosection already is 0x0000
      }

      free(buf);
      return 0;
    }

    free(buf);
  }

  // distinguish the sections
  switch (lmp->flags & sections) {
  case incomment:
    fprintf(stderr, "lmp2atomstyle_parse_line: incomment during parsing");
    return -1;
  case inheaders:
    // read headers
    {
      if (strstr(line, "hi") != NULL && strstr(line, "lo") != NULL) {
        // xlo xhi
        // ignore for now
      } else if (strstr(line, "xy") && strstr(line, "xz") && strstr(line, "yz")) {
        // triclinic factors
        // ignore for now
      } else if (strstr(line, "atoms")) {
        lmp->flags |= atoms;
      } else if (strstr(line, "bonds")) {
        lmp->flags |= bonds;
      } else if (strstr(line, "angles")) {
        lmp->flags |= angles;
      } else if (strstr(line, "dihedrals")) {
        lmp->flags |= dihedrals;
      } else if (strstr(line, "impropers")) {
        lmp->flags |= impropers;
      } else if (strstr(line, "ellipsoids")) {
        lmp->flags |= ellipsoids;
      } else if (strstr(line, "lines")) {
        lmp->flags |= lines;
      } else if (strstr(line, "triangles")) {
        lmp->flags |= triangles;
      } else if (strstr(line, "bodies")) {
        lmp->flags |= bodies;
      } else {
        // unhandled keywords
      }
    }
    break;
  case inatoms:
    if (lmp->numprops == 0) {
      // only read the line if we haven't read the number of props before
      lmp->numprops = (uint8_t)countwords(line);
    }
    break;
  case nosection:
    // wait for a section
    break;
  default:
    fprintf(stderr, "internal error: unknown section flag");
    break;
  }

  return 0;
}

/**
 * check whether the lmphandle is ready to provide a style guess
 * 
 * @return 0 if ready, 1 if not, -1 on failure
 */
extern int lmp2atomstyle_ready(lmphandle handle) {
  if (!handle) {
    return -1;
  }
  // just try it and return 1 on failure
  int ret = lmp2atomstyle_get_style(handle, NULL, 0);
  if (ret != 0) {
    return -1;
  }
  return 0;
}

int apply_style(char *out_style, size_t buflen, const char *style) {
  if (out_style == NULL) {
    return 0;
  }

  if (strlen(style) + 1 > buflen) {
    fprintf(stderr, "lmp2atomstyle_get_style: output buffer out_style too small");
    return -1;
  }

  strcpy(out_style, style);
  return 0;
}

/**
 * Retrieves the atom style and places it in out_style
 * 
 * @return 0 on success
 */
extern int lmp2atomstyle_get_style(lmphandle handle, char *out_style, size_t buflen) {
  if (!handle) {
    return -1;
  }
  struct lmp_t *lmp = (struct lmp_t*)handle;

  // cosmetic shortcut only
  uint16_t f = lmp->flags;
  uint8_t n = lmp->numprops;

  const char *style = NULL;

  switch (f & headers) {
  case 0x007:
    if (n == 6 || n == 9)
      style = "angle";
    break;
  case 0x001:
    if (n == 5)
      style = "atomic";
    if (n == 8)
      style = "atomic/electron/meso";
    if (n == 11)
      style = "electron/meso";
    if (n == 6)
      style = "charge";
    if (n == 7)
      style = "peri/sphere";
    if (n == 9)
      style = "charge/dipole";
    if (n == 10)
      style = "peri/sphere";
    if (n == 12)
      style = "dipole";
    break;
  case 0x101:
    if (n == 7 || n == 10)
      style = "body";
    break;
  case 0x003:
    if (n == 6 || n == 9)
      style = "bond";
    break;
  case 0x021:
    if (n == 7 || n == 10)
      style = "ellipsoid";
    break;
  case 0x01f:
    if (n == 6 || n == 9)
      style = "molecular";
    if (n == 7 || n == 10)
      style = "full";
    break;
  case 0x041:
    if (n == 8 || n == 11)
      style = "line";
    break;
  case 0x081:
    if (n == 8 || n == 11)
      style = "tri";
    break;
  }

  if (style != NULL) {
    return apply_style(out_style, buflen, style);
  }

  return -1;
}
