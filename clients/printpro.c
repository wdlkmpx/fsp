    /*********************************************************************\
    *  Copyright (c) 1991 by Wen-King Su (wen-king@vlsi.cs.caltech.edu)   *
    *                                                                     *
    *  You may copy or modify this file in any manner you wish, provided  *
    *  that this notice is always included, and that you hold the author  *
    *  harmless for any loss or damage resulting from the installation or *
    *  use of this software.                                              *
    \*********************************************************************/

#include "tweak.h"
#include "client_def.h"
#include "c_extern.h"
#include "my-string.h"
#ifdef STDC_HEADERS
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "printpro.h"

#define Y_or_N(y) ((flags & (y)) ?  "Y" : "N")
#define N_or_Y(y) ((flags & (y)) ?  "N" : "Y")
#define Machine(y) ((flags & (y)) ? "YOU" : "other")

int print_pro PROTO2(UBUF *, ub, FILE *, where)
{
  char flags;
  unsigned len, len1;
  char *pro1, *pro2;

  len = BB_READ2(ub->bb_len);
  len1 = BB_READ4(ub->bb_pos);
  pro1 = ub->buf;
  pro2 = ub->buf+len;

  if(len1) {
    flags = *pro2;
    fprintf(where,"owner: %s, del: %s, create: %s, mkdir: %s, get: %s, list: %s, rename: %s.\n",
	   Machine(DIR_OWNER), Y_or_N(DIR_DEL), Y_or_N(DIR_ADD),
	   Y_or_N(DIR_MKDIR), N_or_Y(DIR_GET), Y_or_N(DIR_LIST),
	   Y_or_N(DIR_RENAME));
  }
  fprintf(where,"%s", pro1);
  fprintf(where,"\n");

  return(0);
}
