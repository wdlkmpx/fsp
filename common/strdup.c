#include "tweak.h"

#ifndef HAVE_STRDUP

#include <stdio.h>
#include "my-string.h"

char *strdup PROTO1(char *, str)
{
  char *nstr;

  if (str == (char*)0) return str;

  nstr = (char*)malloc((unsigned int)(strlen(str) + 1));

  if (nstr == (char*)0) {
    fprintf(stderr, "strdup(): not enough memory to duplicate `%s'\n", str);
    exit(1);
  }

  strcpy(nstr, str);

  return nstr;
}
#endif
