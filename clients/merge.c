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
#ifdef STDC_HEADERS
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "my-string.h"

/* prototypes */
int fcatcmd_main PROTO2(int, argc, char **, argv);
int fcdcmd_main PROTO2(int, argc, char **, argv);
int fgetcmd_main PROTO2(int, argc, char **, argv);
int fgrabcmd_main PROTO2(int, argc, char **, argv);
int flscmd_main PROTO2(int, argc, char **, argv);
int fmkdir_main PROTO2(int, argc, char **, argv);
int fprocmd_main PROTO2(int, argc, char **, argv);
int fput_main PROTO2(int, argc, char **, argv);
int frmcmd_main PROTO2(int, argc, char **, argv);
int frmdircmd_main PROTO2(int, argc, char **, argv);
int fver_main PROTO2(int, argc, char **, argv);
int fducmd_main PROTO2(int, argc, char **, argv);
int fhostcmd_main PROTO2(int, argc, char **, argv);
int ffindcmd_main PROTO2(int, argc, char **, argv);
int fstatcmd_main PROTO2(int, argc, char **, argv);

int main PROTO2(int, argc, char **, argv)
{
  char *p, *q;

  for(p = q = argv[0]; *p; p++) if(*p == '/') q = p+1;
  
  if(!strcmp(q,"fcatcmd")) fcatcmd_main(argc,argv);
  else if(!strcmp(q,"fcdcmd")) fcdcmd_main(argc,argv);
  else if(!strcmp(q,"fgetcmd")) fgetcmd_main(argc,argv);
  else if(!strcmp(q,"fgrabcmd")) fgrabcmd_main(argc,argv);
  else if(!strcmp(q,"flscmd")) flscmd_main(argc,argv);
  else if(!strcmp(q,"fmkdir")) fmkdir_main(argc,argv);
  else if(!strcmp(q,"fprocmd")) fprocmd_main(argc,argv);
  else if(!strcmp(q,"fput")) fput_main(argc,argv);
  else if(!strcmp(q,"frmcmd")) frmcmd_main(argc,argv);
  else if(!strcmp(q,"frmdircmd")) frmdircmd_main(argc,argv);
  else if(!strcmp(q,"fver")) fver_main(argc,argv);
  else if(!strcmp(q,"fducmd")) fducmd_main(argc,argv);
  else if(!strcmp(q,"fhostcmd")) fhostcmd_main(argc,argv);
  else if(!strcmp(q,"ffindcmd")) ffindcmd_main(argc,argv);
  else if(!strcmp(q,"fstatcmd")) fstatcmd_main(argc,argv);
  else {
    fprintf(stderr,"Unknown FSP client command: %s\n",q);
    exit(1);
  }
  exit(0);
}