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
#include "co_extern.h"
#ifdef STDC_HEADERS
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "my-string.h"
#include "merge.h"

static int timestamps=0;
static int optletter;

static void usage (void)
{
    printf("fput");
    printf(" [-p | [ -h | -? ] ] file ...\n");
}

static int put_file (char * path)
{
  struct stat sb;
  char *name, *t2;
  FILE *fp;

  if(stat(path,&sb) != 0) {
    perror(path);
    return(0);
  }
  if(!(S_ISREG(sb.st_mode))) {
    fprintf(stderr,"%s: not a file\n",path);
    return(0);
  }

  for(name = t2 = path; *t2; t2++)
    if(*t2 == '/') name = t2 + 1;

  if( (fp = fopen(path,"rb"))) {
    util_upload(name,fp,timestamps==1?sb.st_mtime:0);
    fclose(fp);
  } else fprintf(stderr,"Cannot read %s\n",path);

  return(0);
}

int main (int argc, char ** argv)
{
  char n[1024];
  int prompt;

  env_client();
  if (strcmp(env_local_dir,".") && chdir(env_local_dir)) {
    perror("chdir");
    exit(1);
  }

  while ((optletter=getopt(argc, argv,"ph?")) != EOF)
    switch (optletter) {
      case 'h':
      case '?':
	       usage();
	       exit(0);
      case 'p':
	       timestamps=1;
    }
  if(argc > optind)
    while(*++argv) put_file(*argv);
  else {
    prompt = isatty(0);
    while(1) {
      if(prompt) {
	fputs("fput: ",stdout);
	fflush(stdout);
      }
      if(!getsl(n,1024)) break;
      if(!*n) continue;
      put_file(n);
    }
  }

  client_done();

  exit(0);
}
