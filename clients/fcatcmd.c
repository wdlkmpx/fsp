    /*********************************************************************\
    *  Copyright (c) 2003 by Radim Kolar (hsn@cybermail.net)              *
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
#include "bsd_extern.h"
#ifdef STDC_HEADERS
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

static RETSIGTYPE dont_die PROTO1(int, signum)
{
#ifndef RELIABLE_SIGNALS	
  signal(SIGPIPE,dont_die);
#endif
}

int main PROTO2(int, argc, char **, argv)
{
  char **av;

  env_client();

  signal(SIGPIPE,dont_die);
  if(isatty(1)) client_trace = 0;

  while(*++argv) {
    av = glob(*argv);
    if(av)
      while(*av)
      {
         util_download(*av,stdout,0);
	 *av++;
      }
  }

  client_done();

  exit(0);
}
