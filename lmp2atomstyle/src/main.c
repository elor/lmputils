#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lmp2atomstyle.h>

#ifndef BUFFERSIZE
#define BUFFERSIZE 1024
#endif

FILE *init(int argc, char **argv)
{
  FILE *ret = NULL;
  switch (argc) {
  case 1:
    return stdin;
  case 2:
    ret = fopen(argv[1], "r");
    if (ret == NULL) {
      perror("cannot open input file");
      exit(1);
    }

    return ret;
  default:
    // should never be triggered
    
    exit(1);
  }

  // should not be executed
  perror("init(): case not caught");
  exit(1);
}

void finish(FILE *input)
{
  if (input == stdin) {
    // nothing to do
  } else {
    fclose(input);
  }
}

int main(int argc, char **argv)
{
  FILE *input = init(argc, argv);

  char *line = NULL;
  size_t charsread = 0;
  size_t linesize = 0;

  lmphandle lmp = lmp2atomstyle_create();

  char style[BUFFERSIZE];
  memset(style, '\0', BUFFERSIZE);

  while (!feof(input)) {
    charsread = getline(&line, &linesize, input);
    if (charsread >= 0) {

      lmp2atomstyle_parse_line(lmp, line);
      free(line);
      line = NULL;

      if (lmp2atomstyle_ready(lmp) == 0) {
        if (lmp2atomstyle_get_style(lmp, style, BUFFERSIZE) == 0) {
          break;
        }
      }

    }
  }

  free(lmp);
  
  finish(input);

  if (strlen(style) == 0) {
    return 1;
  } else {
    printf("%s", style);
    return 0;
  }
}
