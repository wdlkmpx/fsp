/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Michael Fischbein.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, and (2) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by the University of California, Berkeley and its contributors''
 * in the documentation or other materials provided with the distribution
 * and in all advertising materials mentioning features or use of this
 * software. Neither the name of the University nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "tweak.h"
#include "common_def.h"
#include <ctype.h>
#ifdef STDC_HEADERS
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "ls.h"

void prcopy PROTO3(char *, src, char *, dest, int, len)
{
  register int ch;

  while(len--) {
    ch = *src++;
    *dest++ = isprint(ch) ? ch : '?';
  }
}

void nomem PROTO0((void))
{
  (void)fprintf(stderr, "ls: out of memory.\n");
  ls_bad(1);
}

char *emalloc PROTO1(int, size)
{
  char *retval;

  if (!(retval = malloc(size))) nomem();
  return(retval);
}

void usage PROTO0((void))
{
  (void)fprintf(stderr, "usage: ls [-1ACFLRacdfgiklqrstu] [file ...]\n");
  ls_bad(1);
}
