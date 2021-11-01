    /*********************************************************************\
    *  Copyright (c) 1992 by Wen-King Su (wen-king@vlsi.cs.caltech.edu)   *
    *                                                                     *
    *  You may copy or modify this file in any manner you wish, provided  *
    *  that this notice is always included, and that you hold the author  *
    *  harmless for any loss or damage resulting from the installation or *
    *  use of this software.                                              *
    \*********************************************************************/

#include "tweak.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "my-string.h"

/* prototypes */
int fcatcmd_main   (int argc, char ** argv);
int fcdcmd_main    (int argc, char ** argv);
int fgetcmd_main   (int argc, char ** argv);
int fgrabcmd_main  (int argc, char ** argv);
int flscmd_main    (int argc, char ** argv);
int fmkdir_main    (int argc, char ** argv);
int fprocmd_main   (int argc, char ** argv);
int fput_main      (int argc, char ** argv);
int frmcmd_main    (int argc, char ** argv);
int frmdircmd_main (int argc, char ** argv);
int fver_main      (int argc, char ** argv);
int fducmd_main    (int argc, char ** argv);
int fhostcmd_main  (int argc, char ** argv);
int ffindcmd_main  (int argc, char ** argv);
int fstatcmd_main  (int argc, char ** argv);
int fmvcmd_main    (int argc, char ** argv);
int fbye_main      (int argc, char ** argv);
int fsetupcmd_main (int argc, char ** argv);

static void fspcmd_usage (char *app)
{
	printf ("usage: \n");
	printf ("   %s <command> [unknown options]\n\n", app);
	printf ("Supported commands:\n");
	printf ("   bye, cat, cd, du, get, grab\n");
	printf ("   ls, mkdir, mv, pro, put, rm\n");
	printf ("   rmdir, hostcmd, find, stat, setup, ver\n\n");
}

// ================================================

int main (int argc, char ** argv)
{
  char * cmd;
  cmd = argv[0];
  if (argc >= 2) {
     cmd = argv[1];
     argv++;
     argc--;
  } else {
     fspcmd_usage (argv[0]);
     return 0;
  }

  if      (!strcmp(cmd,"bye"))   fbye_main(argc,argv);
  else if (!strcmp(cmd,"cat"))   fcatcmd_main(argc,argv);
  else if (!strcmp(cmd,"cd"))    fcdcmd_main(argc,argv);
  else if (!strcmp(cmd,"du"))    fducmd_main(argc,argv);
  else if (!strcmp(cmd,"get"))   fgetcmd_main(argc,argv);
  else if (!strcmp(cmd,"grab"))  fgrabcmd_main(argc,argv);
  else if (!strcmp(cmd,"ls"))    flscmd_main(argc,argv);
  else if (!strcmp(cmd,"mkdir")) fmkdir_main(argc,argv);
  else if (!strcmp(cmd,"mv"))    fmvcmd_main(argc,argv);
  else if (!strcmp(cmd,"pro"))   fprocmd_main(argc,argv);
  else if (!strcmp(cmd,"put"))   fput_main(argc,argv);
  else if (!strcmp(cmd,"rm"))    frmcmd_main(argc,argv);
  else if (!strcmp(cmd,"rmdir")) frmdircmd_main(argc,argv);
  else if (!strcmp(cmd,"hostcmd")) fhostcmd_main(argc,argv);
  else if (!strcmp(cmd,"find"))    ffindcmd_main(argc,argv);
  else if (!strcmp(cmd,"stat"))    fstatcmd_main(argc,argv);
  else if (!strcmp(cmd,"setup"))   fsetupcmd_main(argc,argv);
  else if (!strcmp(cmd,"ver"))     fver_main(argc,argv);
  else
  {
    fprintf(stderr,"fspmerge: Unknown FSP client command: %s\n", cmd);
    exit(EX_USAGE);
  }
  exit(EX_OK);
}
