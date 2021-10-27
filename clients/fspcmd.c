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

int main (int argc, char ** argv)
{
  char *p, *q;

  for(p = q = argv[0]; *p; p++) if(*p == '/') q = p+1;

  if      (!strcmp(q,"bye"))   fbye_main(argc,argv);
  else if (!strcmp(q,"cat"))   fcatcmd_main(argc,argv);
  else if (!strcmp(q,"cd"))    fcdcmd_main(argc,argv);
  else if (!strcmp(q,"du"))    fducmd_main(argc,argv);
  else if (!strcmp(q,"get"))   fgetcmd_main(argc,argv);
  else if (!strcmp(q,"grab"))  fgrabcmd_main(argc,argv);
  else if (!strcmp(q,"ls"))    flscmd_main(argc,argv);
  else if (!strcmp(q,"mkdir")) fmkdir_main(argc,argv);
  else if (!strcmp(q,"mv"))    fmvcmd_main(argc,argv);
  else if (!strcmp(q,"pro"))   fprocmd_main(argc,argv);
  else if (!strcmp(q,"put"))   fput_main(argc,argv);
  else if (!strcmp(q,"rm"))    frmcmd_main(argc,argv);
  else if (!strcmp(q,"rmdir")) frmdircmd_main(argc,argv);
  else if (!strcmp(q,"hostcmd")) fhostcmd_main(argc,argv);
  else if (!strcmp(q,"find"))    ffindcmd_main(argc,argv);
  else if (!strcmp(q,"stat"))    fstatcmd_main(argc,argv);
  else if (!strcmp(q,"setup"))   fsetupcmd_main(argc,argv);
  else if (!strcmp(q,"ver"))     fver_main(argc,argv);
  else
  {
    fprintf(stderr,"fspmerge: Unknown FSP client command: %s\n",q);
    fprintf(stderr,"This program is single merged executable for invoking FSP client commands.\n"
                   "It executes different FSP commands based on invoked name. Example:\n"
		   "If fspmerge is invoked using fver executable name it will execute fver command\n"
		   "and exit. Executable name can be set using symlink ln -s fspmerge fver or\n"
		   "setting argv[0] passed to execve function.\n"
		   "Using merged fsp client over single client executables saves diskspace,\n"
		   "memory and have shorter startup time.\n");
   exit(EX_USAGE);
  }
  exit(EX_OK);
}
