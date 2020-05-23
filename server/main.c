    /*********************************************************************\
    *  Copyright (c) 2003-2005  by Radim Kolar (hsn@sendmail.cz  )        *
    *  Copyright (c) 1991 by Wen-King Su (wen-king@vlsi.cs.caltech.edu)   *
    *                                                                     *
    *  You may copy or modify this file in any manner you wish, provided  *
    *  that this notice is always included, and that you hold the author  *
    *  harmless for any loss or damage resulting from the installation or *
    *  use of this software.                                              *
    \*********************************************************************/

#include "tweak.h"
#include "server_def.h"
#include "s_extern.h"
#include "co_extern.h"
#include "my-string.h"
#include "pidfile.h"

#define NULL_DEV "/dev/null"

static int inetd_mode = 0;
static const char *config_file = CONF_FILE ;

static void display_version (void)
{
      printf(
#ifndef LAMERPACK      
          "File Service Protocol Daemon - FSP "PACKAGE_VERSION"\n"
	  "Copyright (c) 1991-1996 by A. J. Doherty, 2001-2005 by Radim Kolar.\n"
	  "All of the FSP code is free software with revised BSD license.\n"
	  "Portions copyright by BSD, Wen-King Su, Philip G. Richards, Michael Meskes.\n"
#ifdef __GNUC__
	  "Compiled "__DATE__" by GCC "__VERSION__"\n"
#endif
#else
	  "FSP server "PACKAGE_VERSION"\n"
	  "Antiscan protection actived!\n"
	  "For lamah by FSP Gods!\n"
#endif
	    );
}

/* flush the log message to file */

static void arg_err (void)
{
#ifndef LAMERPACK
  fputs("Usage: fspd [-f configfile] [-d directory] [-v|-V] [-i] [-F] [-p port] [-X] [-t inetd timeout] [-T temporary directory] [-l logfile] [-P pidlogname] [-b bytes/sec] [-s packetsize]\n", stderr);
#else
  fputs("Usage: fspd [-d directory] [-p port] [-T temporary directory] [-l logfile] [-b bytes/sec] [-s packetsize]\n", stderr);
#endif
}

static void check_required_vars (void)
{
  double rnd;

#ifdef LAMERPACK
  inetd_mode = 0;
  daemonize = 0;
  dbug = 0;
  dir_cache_limit = 500;
  udp_port = 80;
#endif

  if(!inetd_mode && udp_port==0) {
#ifdef LAMERPACK
#else
    fprintf(stderr, "Error: No port set. (Use 65535 for random port)\n");
    exit(1);
#endif
  }
  if(udp_port == 65535)
  {
      /* generate random port in 1024-65535 range */
      rnd=(random())/(double)RAND_MAX;
      udp_port=rnd*(65535-1024)+1024;
  }
  if(packetsize > UBUF_MAXSPACE)
      packetsize = UBUF_MAXSPACE;
  else
      if (packetsize == 0)
	  packetsize = DEFAULT_SPACE;
      else
	  if(packetsize < 64)
	      packetsize = 64;

  if(!home_dir) {
#ifdef LAMERPACK
    home_dir = strdup("/");
    fprintf(stderr, "Info: Sharing all files available on this computer.\n");
#else
    fprintf(stderr, "Error: No home directory set.\n");
    exit(1);
#endif
  }
#if 0
  if(*home_dir != '/') {
    fprintf(stderr,"Error: home directory [%s] does not start with a /.\n", home_dir);
    exit(1);
  }
#endif
#if 0
  if(!pidlogname) {
    fprintf(stderr, "No pidlogname set in your fspd.conf.\n");
    exit(1);
  }
#endif
  if(!readme_file) {
    readme_file = strdup(".README");
  }
  if(logging && !logname) {
    logging=0;
  }
  if(daemonize && dbug)
      daemonize = 0;
  if(inetd_mode && dbug)
      dbug = 0;
  if(!tmp_dir && !read_only)
  {
#ifndef LAMERPACK
      if(!inetd_mode)
	  fprintf(stderr,"Warning: no tmpdir set, switching to readonly mode.\n");
#else

	  fprintf(stderr,"Info: Writes disabled because tmpdir not set.\n");
#endif
      read_only = 1;
  }
}

static void init_random (void)
{
#ifdef HAVE_SRANDOMDEV
  srandomdev();
#else
  unsigned int seed;
  FILE *f;

  f=fopen("/dev/urandom","rb");
  if(f)
  {
      fread(&seed,sizeof(unsigned int),1,f);
      fclose(f);
  } else
      seed=getpid()*time(NULL);

  srandom(seed);
#endif
}

int main (int argc, char ** argv)
{
  int opt;
  long inetd_timeout=0;

  if(strlen(argv[0])>=7)
	  inetd_mode = !strcasecmp(&argv[0][strlen(argv[0])-7],"in.fspd");

  /* we need to check if we have config file at command line */
  for(opt=1;opt<argc-1;opt++)
  {
      /* printf("arg %d = %s\n",opt,argv[opt]); */
      if(!strcmp(argv[opt],"-f"))
      {
	  load_configuration(argv[opt+1]);
	  config_file=NULL;
      }
  }

  load_configuration(config_file);

  while( (opt=getopt(argc,argv,
#ifndef LAMERPACK
      "h?Xd:f:vVip:t:FT:l:P:b:s:"
#else
      "d:p:T:l:b:h?s:"
#endif
                                ))!=EOF)
  {
	  switch(opt)
	  {
		  case 'X':
			  dbug = 1;
			  break;
		  case 'f':
		  	  /* already loaded */
			  break;
		  case 'd':
			  if(home_dir) free(home_dir);
			  home_dir = strdup(optarg);
			  break;
		  case 'l':
			  if(logname) free(logname);
			  logname = strdup(optarg);
			  logging = L_ALL ^ L_RDONLY;
			  break;
	          case 'P':
		          if(pidlogname) free(pidlogname);
		          pidlogname = strdup(optarg);
		          break;
		  case 'T':
			  if(tmp_dir)
			      free(tmp_dir);
			  tmp_dir = strdup(optarg);
			  break;
		  case 'i':
			  inetd_mode = 1;
			  break;
		  case 'p':
			  udp_port = atoi (optarg);
			  break;
		  case 's':
			  packetsize = atoi (optarg);
			  break;
		  case 'b':
			  maxthcallowed = atoi (optarg);
			  break;
		  case 't':
			  inetd_timeout = 1000L * atoi (optarg);
			  break;
		  case 'F':
			  daemonize = 0;
			  break;
		  case 'v':
		  case 'V':
			  display_version();
			  exit(0);
		  case 'h':
		  case '?':
		  	  arg_err();
			  exit(0);
		  default:
		  	  arg_err();
			  exit(1);
	  }
  }

  init_random();
  check_required_vars();

  if(!inetd_mode)
  {
    opt=_x_udp(listen_on,&udp_port);
    if(opt == -1) {
    fprintf(stderr,"can't listen on port %d: ",udp_port);
    fflush(stderr);
    perror("");
    exit(2);
    }
    if(dbug) {
        display_version();
	fprintf(stderr,"listening on port %d\n",udp_port);
	fprintf(stderr,"FSP payload size %d bytes\n",packetsize);
    }
#ifdef LAMERPACK
    display_version();
    fprintf(stderr,"rocking on port %d\n",udp_port);
    fprintf(stderr,"FSP payload size %d bytes\n",packetsize);
#endif
  }

  /* Moved setuid to here from below because log file was getting opened
   * by root, and fspd could no longer write to the log file after the
   * setuid. This should always open the file as run_uid
   * Modified A.E.J.Fellows 9 March 93
   */

  if(run_uid) if(setuid(run_uid) != 0) {
      fprintf(stderr,"Can not change my uid to %d.\n",run_uid);
      exit(3);
  }

  if(run_gid) if(setgid(run_gid) != 0) {
      fprintf(stderr,"Can not change my gid to %d.\n",run_uid);
      exit(4);
  }

  init_home_dir();

  if(init_caches())
  {
      perror("init_caches");
      exit(5);
  }

  umask(system_umask);

  if (logging) {
#ifndef LAMERPACK
     if (dbug)
#endif
	 fprintf(stderr,"logging to %s\n",logname);
    /* test to see if logfile can be written */
    /* open it append mode so that it doesn't wipe the file when
     * you are running under inetd.
     */
    if((logfd=open(logname, O_WRONLY | O_APPEND | O_CREAT, 0640)) < 0) {
	if(! inetd_mode )
	    fprintf(stderr, "Error opening logfile: %s, logging disabled.\n",
	      logname);
      logging = 0; /* no logging */
    }
  }

  if(tlogname)
  {
     if (dbug)
	 fprintf(stderr,"logging transfers to %s\n",tlogname);

    /* test to see if logfile can be written */
    if((tlogfd=open(tlogname, O_WRONLY | O_APPEND | O_CREAT, 0640)) < 0)
    {
	if(! inetd_mode )
	    fprintf(stderr, "Error opening transferfile: %s, transfer logging disabled.\n",tlogname);
        free(tlogname);
        tlogname=NULL; /* no logging */
    }
  }

  /* With pidfile we have currently 2 problems:
     1) creating pidfile after we have droped root rights. We can not
        write to root only directories like /var/run
     2) If we create pidfile early before setuid() we can't write
        new pid to it after we setuid()+fork()
  */
#ifndef LAMERPACK  
  if (pidfile(pidlogname)) {
	  fprintf(stderr,"Error: can not write pidfile %s - exiting.\n",pidlogname);
	  exit(8);/* cannot write pid file - exit */
  }
#endif
  init_htab();
  /* we can enable table dumping from there */
  signal(SIGINT,server_interrupt);
  signal(SIGTERM,server_interrupt);
  signal(SIGUSR1,server_dump);

  /* set timeouts */
  if(inetd_mode)
  {
      if(inetd_timeout==0)
	  /* 5. minutes is maximum resend timeout required by protocol */
	  inetd_timeout=5*60*1000L;
  }else
  {
      if(inetd_timeout==0 || !dbug)
	  inetd_timeout=-1L;
  }

  /* inetd init */
  if(inetd_mode) {
    opt=dup(0);
  }

  if(daemonize || inetd_mode)
  {
    freopen(NULL_DEV,"r",stdin);
    freopen(NULL_DEV,"w",stdout);
    freopen(NULL_DEV,"w",stderr);
  }


  if(!inetd_mode) {
    /* Fork and die to drop daemon into background   */
    /* Added Alban E J Fellows 12 Jan 93             */
    /* Moved by JT Traub to only do this if not running under inetd. */
    if(daemonize) {
#if HAVE_FORK
      pid_t forkpid;
      forkpid = fork();
      if (forkpid == 0) { /* child prozess */
	if (pidfile(pidlogname)) {
          pidfile_cleanup(pidlogname); /* try cleanup */
	  exit(1);/* cannot write pid file - exit */
	}
      } else if (forkpid > 0) { /* father prozess */
	_exit(0);
      }
#endif
#if HAVE_SETSID
      setsid();
#endif
    }
  }

  while(1)
  {
    server_loop(opt,inetd_timeout);
    if(inetd_mode||dbug||shutdowning) break;
  }

  pidfile_cleanup(pidlogname);
  shutdown_caches();
  destroy_configuration();
  exit(0);
}
