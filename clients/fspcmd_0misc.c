    /*********************************************************************\
    *  Copyright (c) 1991 by Wen-King Su (wen-king@vlsi.cs.caltech.edu)   *
    *  Copyright (c) 2005 by Radim Kolar (hsn@cybermail.net)              *
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
#include "co_extern.h"
#include "my-string.h"
#include "ls.h"
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "fspcmd_0misc.h"
#include "find.h"

#define Y_or_N(y) ((flags & (y)) ?  "Y" : "N")
#define N_or_Y(y) ((flags & (y)) ?  "N" : "Y")
#define Machine(y) ((flags & (y)) ? "YOU" : "other")

int print_pro (UBUF * ub, FILE * where)
{
  char flags;
  unsigned len, len2;
  char *pro1, *pro2;
  char *c;

  /* length of readme */
  len = BB_READ2(ub->bb_len);
  /* len2= size of extended protection data */
  len2 = BB_READ4(ub->bb_pos);
  pro1 = ub->buf;
  pro2 = ub->buf+len;
  if(len2) {
    flags = *pro2;
    fprintf(where,"owner: %s, del: %s, create: %s, mkdir: %s, get: %s, list: %s, rename: %s.\n",
	   Machine(DIR_OWNER), Y_or_N(DIR_DEL), Y_or_N(DIR_ADD),
	   Y_or_N(DIR_MKDIR), N_or_Y(DIR_GET), Y_or_N(DIR_LIST),
	   Y_or_N(DIR_RENAME));
  }
  if(len>1) 
  {
     /* remove trailing \n\r from README */
     c=pro1+strlen(pro1)-1;
     while(*c=='\n' || *c=='\r')
        c--;
     *c='\0';

     fprintf(where,"%s\n", pro1);
  }

  return(0);
}

//========================================================================
//                         cmd bye
//========================================================================

int fbye_main (int argc, char ** argv)
{
    env_client();
    client_finish();
    return 0;
}


//========================================================================
//                         cmd cat
//========================================================================

static void dont_die (int signum)
{
#ifndef RELIABLE_SIGNALS	
  signal(signum,dont_die);
#endif
}

int fcatcmd_main (int argc, char ** argv)
{
  char **av;
  env_client();

  signal(SIGPIPE,dont_die);
  if(isatty(1)) {
      client_trace = 0;
  } else {
      signal(SIGHUP,dont_die);    
  }
  while(*++argv) {
    av = bsdglob(*argv);
    if(av)
      while(*av)
      {
         util_download(*av,stdout,0);
	 *av++;
      }
  }

  client_done();
  exit(EX_OK);
}


//========================================================================
//                         cmd cd
//========================================================================

static int f_cd (char * p)
{
  UBUF *ub;
  ub = client_interact(CC_GET_PRO,0L, strlen(p), (unsigned const char *)p+1, 0,
                       (unsigned char *)NULLP);
  if(ub->cmd == CC_ERR) {
    fprintf(stderr, "ERR: %s\n",ub->buf);
    return(0);
  } else {
    util_junk_password(p);
    fprintf(stderr, "directory %s\nmode: ",p);
    print_pro(ub,stderr);
    return(1);
  }
}

int fcdcmd_main (int argc, char ** argv)
{
  char *np;
  char **av, *av2[2];

  env_client();
  if(argc == 1) {
    np=util_abs_path("/");  
    f_cd(np);
    puts(np);
  } else {
    if(!(av = bsdglob(argv[1]))) {
      av = av2;
      av2[0] = *argv;
      av2[1] = 0;
    }
    np = util_abs_path(*av);
    if(f_cd(np)) {
       puts(np);
    } else {
      puts(env_dir);
    }
  }
  client_done();
  exit(EX_OK);
}


//========================================================================
//                         cmd ls
//========================================================================

int ls_bad (int n)
{
  client_done();
  exit(n);
}

int flscmd_main (int argc, char ** argv)
{
  env_client();
  fls_main(argc,argv);
  client_done();
  exit(EX_OK);
}


//========================================================================
//                         cmd mkdir
//========================================================================

static int make_dir (char * p)
{
  char *op = util_abs_path(p);
  UBUF *ub = client_interact(CC_MAKE_DIR,0L, strlen(op), (unsigned char *)op+1, 0,
                           (unsigned char *)NULLP);
  if(ub->cmd == CC_ERR) {
    fprintf(stderr,"Can't create %s: %s\n",p,ub->buf);
    free(op);
    return(-1);
  }
  printf("%s\t: %s\n",p,ub->buf);
  free(op);
  return(0);
}

int fmkdir_main (int argc, char ** argv)
{
  env_client();
  while(*++argv) {
     make_dir(*argv);
  }
  client_done();
  exit(EX_OK);
}


//========================================================================
//                         cmd pro
//========================================================================

static int get_pro (const char * p)
{
  char *op = util_abs_path(p);
  UBUF *ub = client_interact(CC_GET_PRO,0L, strlen(op), (unsigned char *)op+1, 0,
                             (unsigned char *)NULLP);
  if(ub->cmd == CC_ERR) {
    fprintf(stderr, "ERR: %s\n",ub->buf);
    return(1);
  }
  printf("%s\n",p);
  print_pro(ub,stdout);
  return(0);
}

static int set_pro (char * p, char * key)
{
  char *op = util_abs_path(p);
  UBUF *ub = client_interact(CC_SET_PRO,strlen(key), strlen(op), (unsigned char *)op+1,
                             strlen(key), (unsigned char *)key);
  if(ub->cmd == CC_ERR) {
    fprintf(stderr, "ERR: %s\n",ub->buf);
    return(1);
  }
  printf("%s\n",p);
  print_pro(ub,stdout);
  return(0);
}

int fprocmd_main (int argc, char ** argv)
{
  char **av, *av2[2], *key;

  env_client();

  if(argv[1] && (argv[1][0] == '+' || argv[1][0] == '-') && !argv[1][2]) {
    /* set pro command */
    if (argc > 2)
    {
	key = *++argv;
	while(*++argv) {
	  if(!(av = bsdglob(*argv))) {
	    av = av2;
	    av2[0] = *argv;
	    av2[1] = 0;
	  }
	  while(*av) set_pro(*av++,key);
	}
    }
    else set_pro(env_dir,key);
  } else {
    /* get pro command */  
    if(argv[1]) while(*++argv) {
      if(!(av = bsdglob(*argv))) {
	av = av2;
	av2[0] = *argv;
	av2[1] = 0;
      }
      while(*av) get_pro(*av++);
    } else get_pro(env_dir);
  }
  client_done();
  exit(EX_OK);
}


//========================================================================
//                         cmd rm
//========================================================================

static int remove_file (char * p)
{
  char *op = util_abs_path(p);
  UBUF *ub =   ub = client_interact(CC_DEL_FILE,0L, strlen(op), (unsigned char *)op+1, 0,
                        (unsigned char *)NULLP);
  if(ub->cmd == CC_ERR) {
    fprintf(stderr,"Can't remove %s: %s\n",p,ub->buf);
    free(op); return(-1);
  }
  return(0);
}

int frmcmd_main (int argc, char ** argv)
{
  char **av, *av2[2];
  env_client();
  while(*++argv) {
    if(!(av = bsdglob(*argv))) {
      av = av2;
      av2[0] = *argv;
      av2[1] = 0;
    }
    while(*av) remove_file(*av++);
  }
  client_done();
  exit(EX_OK);
}


//========================================================================
//                         cmd rmdir
//========================================================================

static int remove_dir (char * p)
{
  char *op = util_abs_path(p);
  UBUF *ub = ub = client_interact(CC_DEL_DIR,0L, strlen(op), (unsigned char *)op+1, 0,
                                 (unsigned char *)NULLP);
  if(ub->cmd == CC_ERR) {
    fprintf(stderr,"Can't remove %s: %s\n",p,ub->buf);
    free(op);
    return(-1);
  }
  return(0);
}

int frmdircmd_main (int argc, char **argv)
{
  char **av, *av2[2];
  env_client();
  while(*++argv) {
    if(!(av = bsdglob(*argv))) {
      av = av2;
      av2[0] = *argv;
      av2[1] = 0;
    }
    while(*av) remove_dir(*av++);
  }
  client_done();
  exit(EX_OK);
}


//========================================================================
//                         cmd stat
//========================================================================

static void stat_file (const char *fname)
{
   struct stat sb;
   struct tm *ftime;
   char buf[35];

   if(util_stat(fname,&sb)) {
       printf("%s: stat error\n",fname);
   } else {
       if(S_ISREG(sb.st_mode))
	    printf("File");
       else if(S_ISDIR(sb.st_mode))
	    printf("Directory");

       ftime=localtime(&sb.st_mtime);
       strftime(buf,35,"%Y-%m-%d %H:%M:%S",ftime);
#ifdef NATIVE_LARGEFILES
#define SFORM "%llu"
#else
#define SFORM "%lu"
#endif
       printf(": %s  Size: "SFORM" Time: %s\n",fname,sb.st_size,buf);
   }
}

int fstatcmd_main (int argc, char ** argv)
{
  char n[1024];
  int prompt;
  char **av, *av2[2];

  env_client();

  if(argc>1)
  {
    for( optind=1; argc>optind ; optind++)
    {
      if(!(av = bsdglob(argv[optind])))
      {
        av = av2;
        av2[0] = argv[optind];
        av2[1] = 0;
      }
      while(*av)
        stat_file(*av++);
    }
  } else {
    prompt = isatty(0);
    while(1) {
      if(prompt) {
         fputs("fstat: ",stdout);
         fflush(stdout);
      }
      if(!getsl(n,1024)) break;
      if(!*n) break;
      if(!(av = bsdglob(n))) {
        av = av2;
        av2[0] = n;
        av2[1] = 0;
      }
      while(*av)
	  stat_file(*av++);
    }
  }
  client_done();
  exit(EX_OK);
}
