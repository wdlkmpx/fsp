    /*********************************************************************\
    *  Copyright (c) 1993 by Michael Meskes                               *
    *  (meskes@ulysses.informatik.rwth-aachen.de)                         *
    *  Copyright (c) 2003 by Radim Kolar (hsn@cybermail.net)              *
    *                                                                     *
    *  You may copy or modify this file in any manner you wish, provided  *
    *  that this notice is always included, and that you hold the author  *
    *  harmless for any loss or damage resulting from the installation or *
    *  use of this software.                                              *
    \*********************************************************************/

#include "tweak.h"
#include "client_def.h"
#include "c_extern.h"
#include <stdio.h>
#include "my-string.h"
#include <pwd.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef STDC_HEADERS
#include <stdlib.h>
#endif
#include <ctype.h>

#ifdef HOST_LOOKUP
#include <netdb.h>
#endif

#define FSP_STAT stat

#define NUMBER 1
#define NAME 2

#include "fhost.h"

static const char *home="/";

static struct fsp_host *host;
static int hostsize=0;
static int tryfile=0;

/* allocate and init fsp_host structure */
struct fsp_host * init_host(void)
{
    struct fsp_host *h;

    h=malloc(sizeof(struct fsp_host));
    if(h==NULL)
    {
	perror("init_host");
	exit(2);
    }
	
    h->hostname=NULL;
    h->hostaddr=NULL;
    h->alias=calloc(1,sizeof(char *));
    h->port=-1;
    h->dir=NULL;
    h->delay=-1;
    h->local_port=-1;
    h->timeout=-1;
    h->trace=-1;
    h->local_dir=NULL;
    h->password=NULL;

    return h;
}

void add_host_alias(struct fsp_host *h, const char *name)
{
    int i=0;
    while(h->alias[i])
	i++;
    h->alias=realloc(h->alias,sizeof(char *)*(i+2));
    h->alias[i]=strdup(name);
    h->alias[i+1]=NULL;
}

void add_host(struct fsp_host *h)
{
    if (hostsize==0)
	host=NULL;
    if(h==NULL) return;
    host=realloc(host,sizeof(struct  fsp_host)*(hostsize+1));
    if(host==NULL)
    {
	perror("host realloc");
	exit(2);
    }
    memcpy(host+hostsize,h,sizeof(struct fsp_host));
    hostsize++;
    return;
}

static struct fsp_host *find_host(const char *name)
{
    int i,j;
    
    if(name==NULL || hostsize==0 ) return NULL;
    for(i=0;i<hostsize;i++)
    {
	if(host[i].hostname)
	    if(!strcmp(host[i].hostname,name)) return &host[i];
	if(host[i].hostaddr)
	    if(!strcmp(host[i].hostaddr,name)) return &host[i];
	j=0;
        while(host[i].alias[j])
	{
	    if(!strcmp(host[i].alias[j],name)) return &host[i];
	    j++;
	}
    }
    return NULL;
}

static void host_usage PROTO0((void)) /* print usage message */
{
  fprintf(stderr,"Usage: fhost [-d delay] [-p local port] [-l local dir]\n");
  fprintf(stderr,"             [-o timeout] [-t trace] [-w password]\n");
  fprintf(stderr,"             [-f filename] [-h [number|name]] [-c | -b]\n");
  fprintf(stderr,"             [host port [directory] | abbreviation]\n");
  exit(0);
}

/* get data out of resource file */
static void parse_prof_file_new PROTO1(const char *, filename)
{
 
  FILE *input=NULL;
  int rc;
 
  if(filename)
  {
      input=fopen(filename,"r");
  }
  
  if(input)
  {
      yyin=input;
      rc=0;
  } else
      rc=yywrap();
  
  if(rc==0)
      yylex();

  if(input)
      fclose(input);
}

static void list_prof_file PROTO0((void)) /* list resource file */
{
  int i;
  for(i=0;i<hostsize;i++)
  {
      printf("host: %s port: %d\n",(host[i].hostname?host[i].hostname  :  host[i].hostaddr),(host[i].port<=0? 21 : host[i].port));
  }
      
  return;
}

int main PROTO2(int, argc, char **, argv)
{
  int optletter,csh,lhost=0;
  register char *p;
  const char *filename=NULL;
  char *log;
  struct passwd *pw=0L;
  struct hostent *hp;
  long addr;
  struct fsp_host *setup=NULL;

  log = (char *)getlogin();
  if (log) pw = getpwnam(log);
  if (!pw) pw = getpwuid(getuid());
  if (pw) {
    csh = !strcmp(pw->pw_shell + strlen(pw->pw_shell) - 3, "csh");
    home = pw->pw_dir;   /* for default search for file .fsp_prof*/
  } else
      home=getenv("HOME");
  
  /*
   * Figure out what shell we're using.  A hack, we look for a shell
   * ending in "csh".
   */
  log=getenv("SHELL");
  if(log)
  {
    csh = !strcmp(log + strlen(log) - 3, "csh");
  }
  
  setup=init_host();
  while ((optletter=getopt(argc, argv,"d:p:l:t:o:f:h:w:bc?")) != EOF)
    switch (optletter) {
      case '?':
        host_usage();
      case 'd':
        setup->delay=atol(optarg); /* FSP_DELAY */
	break;
      case 'p':
	setup->local_port=atol(optarg); /* FSP_LOCAL_PORT */
	break;
      case 'b':
	csh=0;
	break;
      case 'c':
	csh=1;
	break;
      case 'w':
	setup->password=optarg; /* FSP_PASSWORD */
	break;
      case 'l':
	setup->local_dir=optarg; /* FSP_LOCAL_DIR */
	break;
      case 'o':
	setup->timeout=atol(optarg); /* FSP_TIMEOUT */
	break;
      case 't':
	if (!strcmp(optarg,"on")) setup->trace=1;  /* FSP_TRACE */
	else if (!strcmp(optarg,"off")) setup->trace=0;
	else host_usage();
	break;
      case 'f':
	filename=optarg; /* file name */
	break;
      case 'h':
	if (!strcmp(optarg,"number")) lhost=NUMBER; /* host mode */
	else if (!strcmp(optarg,"name")) lhost=NAME;
	else host_usage();
	break;
      default:
	host_usage();
	break;
    }

  if(argc > optind+1 && !filename) { /* host and port and no filename given */
    for (p=argv[optind];!setup->hostname && *p && *p!='\n';p++)
      if (!isdigit(*p) && *p!='.') setup->hostname=argv[optind];
    if (!setup->hostname) setup->hostaddr=argv[optind];
    setup->port=atol(argv[optind+1]);
    if (argc > optind + 1) setup->dir=argv[optind+2]; /* directory given, too */
  } else if (argc > optind) { /* abbreviation given */
    parse_prof_file_new(filename);
    setup=find_host(argv[optind]);
    if(!setup) setup=init_host();
  } else { /* list or set command-line options */
    if (filename || argc==1) {  /* list only */
      parse_prof_file_new(filename);
      list_prof_file();
      exit(0);
    }
  }
  if (setup->delay>=0) {
    if (csh) printf("setenv FSP_DELAY %d;\n",setup->delay);
    else printf("FSP_DELAY=%d;\nexport FSP_DELAY;\n",setup->delay);
  }
  if (setup->local_port>=0) {
    if (csh) printf("setenv FSP_LOCALPORT %d;\n",setup->local_port);
    else printf("FSP_LOCALPORT=%d;\nexport FSP_LOCALPORT;\n",setup->local_port);
  }
  if (setup->trace>=0) {
    if (csh) {
      if (setup->trace) printf("setenv FSP_TRACE;\n");
      else printf("unsetenv FSP_TRACE;\n");
    } else {
      if (setup->trace) printf("FSP_TRACE;\nexport FSP_TRACE;\n");
      else printf("unset FSP_TRACE;\n");
    }
  }
  if (setup->timeout>=0) {
    if (csh) printf("setenv FSP_TIMEOUT %d;\n",setup->timeout);
    else printf("FSP_TIMEOUT=%d;\nexport FSP_TIMEOUT;\n",setup->timeout);
  }
  if (setup->port>=0) {
    if (csh) printf("setenv FSP_PORT %d;\n",setup->port);
    else printf("FSP_PORT=%d;\nexport FSP_PORT;\n",setup->port);
  }
  if (setup->local_dir) {
    if (csh) printf("setenv FSP_LOCAL_DIR ");
    else printf("FSP_LOCAL_DIR=");
    for (p=setup->local_dir;*p && *p!='\n' && *p!=' ';p++) printf("%c",*p);
    if (csh) printf(";\n");
    else printf(";\nexport FSP_LOCAL_DIR;\n");
  }
  if (setup->password) {
    if (csh) printf("setenv FSP_PASSWORD ");
    else printf("FSP_PASSWORD=");
    for (p=setup->password;*p && *p!='\n' && *p!=' ';p++) printf("%c",*p);
    if (csh) printf(";\n");
    else printf(";\nexport FSP_PASSWORD;\n");
  }
  if (setup->hostname || setup->hostaddr) {
    if (csh) printf("setenv FSP_HOST ");
    else printf("FSP_HOST=");
    if(setup->hostname) {
      for(p = setup->hostname; *p && *p!='\n' && *p!= ' '; p++);
      *p = 0;
    }
    if(setup->hostaddr) {
      for(p=setup->hostaddr;*p && *p !='\n' && *p!=' ';p++);
      *p = 0;
    }
    if(lhost==NAME && !setup->hostname) {
#if HOST_LOOKUP
      addr=inet_addr(setup->hostaddr);
      if ( (hp=gethostbyaddr((char *) &addr, sizeof(addr), AF_INET)))
	setup->hostname= (char *)hp->h_name;
#endif
      if (!setup->hostname) lhost=NUMBER;
    }
    if (lhost==NUMBER && !setup->hostaddr) { /* look for number */
#if HOST_LOOKUP
      if ( (hp=gethostbyname(setup->hostname)))
	setup->hostaddr=(char *)inet_ntoa(*(struct in_addr *) * hp->h_addr_list);
#endif
      if (!setup->hostaddr) lhost=NAME;
    }
    if (!lhost) {
      if (setup->hostaddr) lhost=NUMBER;
      else if (setup->hostname) lhost=NAME;
      else {
	fprintf(stderr,"fhost: No host given!");
	exit(1);
      }
    }
    printf("%s", (lhost==NAME)? setup->hostname : setup->hostaddr);
    if (csh) printf(";\n");
    else printf(";\nexport FSP_HOST;\n");
    if (!setup->dir) setup->dir="/"; /* if host is set we need this */
  }
  if (setup->dir) {
    if (csh) printf("setenv FSP_DIR ");
    else printf("FSP_DIR=");
    for (p=setup->dir;*p && *p!='\n' && *p!=' ';p++) printf("%c",*p);
    if (csh) printf(";\n");
    else printf(";\nexport FSP_DIR;\n");
  }

  if (csh) printf("setenv FSP_NAME \"");
  else printf("FSP_NAME=\"");
  if (setup->hostname)
    for (p=setup->hostname;*p && *p!='\n' && *p!=' ';p++) printf("%c",*p);
  if (csh) printf("\";\n");
  else printf("\";\nexport FSP_NAME;\n");
  exit(0);
}

/*
 *      search order: curdir, homedir, sysdir 
 *
 * Returns: 1 for terminating scanner or 0 for switching
 */

int yywrap(void)
{
  char *f2=NULL;
  int rc;
 
  if(yyin!=NULL)
  {
      fclose(yyin);
      yyin=NULL;
  }

  switch(tryfile)
  {
      case 0:
	     /* file in cur. dir */
             yyin=fopen(FSPPROF,"r");
	     break;
      case 1:
	     /* file in home dir */
	     f2=(char *)malloc (strlen(home) + strlen(FSPPROF) + 2);
	     if (!f2) {
		perror("malloc");
		return(1);
	     }
             sprintf (f2,"%s/%s",home,FSPPROF);
	     yyin=fopen(f2,"r");
	     free(f2);
	     break;
      case 2:
	     yyin=fopen(FSPRC,"r");
	     break;
      default:
	     return 1;
  }
  tryfile++;
  if(yyin==NULL)
  {
      /* try next available */
      rc=yywrap();
      return rc;
  }
      
  return 0;  
}
