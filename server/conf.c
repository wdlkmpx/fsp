    /*********************************************************************\
    *  Copyright (c) 2003-5 by Radim Kolar (hsn@cybermail.net)            *
    *  Copyright (c) 1991 by Wen-King Su (wen-king@vlsi.cs.caltech.edu)   *
    *                                                                     *
    *  You may copy or modify this file in any manner you wish, provided  *
    *  that this notice is always included, and that you hold the author  *
    *  harmless for any loss or damage resulting from the installation or *
    *  use of this software.                                              *
    \*********************************************************************/

#include "tweak.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "server_def.h"
#include "s_extern.h"
#include "co_extern.h"
#include "my-string.h"

static int conf_read = 0;

int daemonize = 1;
int use_prebuild_dirlists =
#ifdef OS_CYGWIN
0;
#else
1;
#endif
int grab_enabled = 1;
int ver_enabled = 1;
int read_only = 0;
unsigned short udp_port = 21;
int dbug = 0;
uid_t run_uid = 0;
gid_t run_gid = 0;
int priv_mode = 0;
int no_unnamed = 0;
int logging = 0;
int use_access_files = 1;
int use_directory_mtime =
#ifdef OS_CYGWIN
0;
#else
1;
#endif
unsigned int maxthcallowed = 0;
unsigned short packetsize = DEFAULT_SPACE;
time_t retry_timeout = 3;
time_t session_timeout = 60;
time_t stat_cache_timeout = 25;
char *logname = NULL;
char *tlogname = NULL;
char *pidlogname = NULL;
char *dumpname = NULL;
char *home_dir = NULL;
char *tmp_dir = NULL;
char *listen_on = NULL;
int homedir_restricted = 1;
int permit_passwordless_owners = 0;
char *readme_file = NULL;
unsigned int dir_cache_limit = DEFAULT_DIRLISTCACHE_SIZE;
unsigned int stat_cache_limit = DEFAULT_DIRSTATCACHE_SIZE;
mode_t upload_umask = 0033;
mode_t system_umask = 0077;
unsigned int fp_cache_limit= DEFAULT_FPCACHE_SIZE;

static void log_set (int flag, int neg)
{
  if(neg) logging &= ~flag;
  else logging |= flag;
}

static int get_boolean (const char *q)
{
      if(strcmp(q, "0") == 0) return 0;
      else
      if(strcasecmp(q, "no") == 0) return 0;
      else
      if(strcasecmp(q, "off") == 0) return 0;
      else
      if(strcasecmp(q, "on") == 0) return 1;
      else
      if(strcasecmp(q, "yes") == 0) return 1;
      else
      if(strcmp(q, "1") == 0) return 1;
      else
	
      fprintf(stderr,"Bogus boolean value '%s'. Exiting.\n",q);
      exit(1);
}

static void read_configuration (const char * name)
{
  FILE *fp;
  char buf[1024], *p, *q;

  fp = fopen(name,"r");
  if(!fp) {
     fprintf(stderr, "Warning: Unable to open configuration file: %s.\n", name);
    return;
  }
  while(fgets(buf, sizeof(buf), fp)) {
    if(buf[0] == '\n' || buf[0] == '#') continue;
    buf[strlen(buf)-1] = '\0'; /* strip off the newline */
    p = buf; while(*p && isspace(*p)) p++;
    q = p; while(*q && !isspace(*q)) q++;
    if(!*p || !*q) {
      fprintf(stderr,"Bogus line in configuration file: %s. Exiting.\n",name);
      exit(1);
    }
    *q = '\0'; q++;
    if(strcasecmp(p, "conf") == 0) {
      if(conf_read) {
	fprintf(stderr, "No recursion of conf commands allowed. Skipping.\n");
	continue;
      }
      conf_read = 1;
      fclose(fp);
      read_configuration(q);
      return;
    } else if(strcasecmp(p, "readme") == 0) {
      if(readme_file) free(readme_file);
      readme_file = strdup(q);
    } else if(strcasecmp(p, "ListenAddress") == 0) {
      if(listen_on) free(listen_on);
      listen_on = strdup(q);
    } else if(strcasecmp(p, "homedir") == 0) {
      if(home_dir) free(home_dir);
      home_dir = strdup(q);
    } else if(strcasecmp(p, "tmpdir") == 0) {
      if(tmp_dir) free(tmp_dir);
      tmp_dir = strdup(q);
    } else if(strcasecmp(p, "logfile") == 0) {
      if(logname) free(logname);
      logname = strdup(q);
    } else if(strcasecmp(p, "xferlog") == 0) {
      if(tlogname) free(tlogname);
      tlogname = strdup(q);
    } else if(strcasecmp(p, "pidlogname") == 0) {
      if(pidlogname) free(pidlogname);
      pidlogname = strdup(q);
    } else if(strcasecmp(p, "dumpfile") == 0) {
      if(dumpname) free(dumpname);
      dumpname = strdup(q);
    } else if(strcasecmp(p, "host") == 0) {
      add_ipline(q,&iptab);
    } else if(strcasecmp(p, "log") == 0) {
      char *r;
      int neg;
      do {
	/* skip to next token */
	r = q; while(*r && !isspace(*r)) r++;
        if (*r) { *r++ = 0 ; while(*r && isspace(*r)) r++; }
        if(strcasecmp(q, "none") == 0) {
	  logging = L_NONE;
	  break;
	} else if(strcasecmp(q, "all") == 0) {
	  logging = L_ALL;
	} else {
	  if(*q == '!') { neg = 1; q++;} else neg = 0;
          if(strcasecmp(q, "transfers") == 0) {
	    log_set(L_GETFILE, neg);
	    log_set(L_INSTALL, neg);
          } else if(strcasecmp(q, "version") == 0) log_set(L_VER, neg);
	  else if(strcasecmp(q, "errors") == 0) log_set(L_ERR, neg);
	  else if(strcasecmp(q, "getdir") == 0) log_set(L_GETDIR, neg);
	  else if(strcasecmp(q, "getfile") == 0) log_set(L_GETFILE, neg);
	  else if(strcasecmp(q, "upload") == 0) log_set(L_UPLOAD, neg);
	  else if(strcasecmp(q, "install") == 0) log_set(L_INSTALL, neg);
	  else if(strcasecmp(q, "delfile") == 0) log_set(L_DELFILE, neg);
	  else if(strcasecmp(q, "deldir") == 0) log_set(L_DELDIR, neg);
	  else if(strcasecmp(q, "setpro") == 0) log_set(L_SETPRO, neg);
	  else if(strcasecmp(q, "getpro") == 0) log_set(L_GETPRO, neg);
	  else if(strcasecmp(q, "makedir") == 0) log_set(L_MAKEDIR, neg);
	  else if(strcasecmp(q, "grabfile") == 0) log_set(L_GRABFILE, neg);
	  else if(strcasecmp(q, "readonly") == 0) log_set(L_RDONLY, neg);
	  else if(strcasecmp(q, "stat") == 0) log_set(L_STAT, neg);
	  else if(strcasecmp(q, "rename") == 0) log_set(L_RENAME, neg);
        }
	q = r;
      } while (*q);
    } else if(strcasecmp(p, "port") == 0)  {
       udp_port = atoi(q);
    }
    else if(strcasecmp(p, "packetsize") == 0) {
      packetsize = atoi(q);
    }
    else if(strcasecmp(p, "filecache") == 0) {
      fp_cache_limit = atoi(q);
      if(fp_cache_limit < 10 )  fp_cache_limit = 10;
    }
    else if(strcasecmp(p, "dircache") == 0) {
      dir_cache_limit = atoi(q);
      if( dir_cache_limit < 16 ) dir_cache_limit = DEFAULT_DIRLISTCACHE_SIZE;
    }
    else if(strcasecmp(p, "statcache") == 0) {
      stat_cache_limit = atoi(q);
      if( stat_cache_limit < 10 ) stat_cache_limit = DEFAULT_DIRSTATCACHE_SIZE;
    }
    else if(strcasecmp(p, "retry") == 0) {
      retry_timeout = atoi(q);
      if(retry_timeout < 1 ) retry_timeout = 3;
    }
    else if(strcasecmp(p, "timeout") == 0) {
      session_timeout = atoi(q);
      if(session_timeout < 7 ) session_timeout = 60;
    }
    else if(strcasecmp(p, "statcache_timeout") == 0) {
      session_timeout = atoi(q);
      if(stat_cache_timeout <= 0 ) stat_cache_timeout = 20;
    }
    else if(strcasecmp(p, "thruput") == 0) {
      if(strcasecmp(q, "off") == 0) maxthcallowed = 0;
      else maxthcallowed = atoi(q);
    } else if(strcasecmp(p, "setuid") == 0) {
      if(strcasecmp(q, "off") == 0) run_uid = 0;
      else run_uid = atoi(q);
    } else if(strcasecmp(p, "setgid") == 0) {
      if(strcasecmp(q, "off") == 0) run_gid = 0;
      else run_gid = atoi(q);
    } else if(strcasecmp(p, "umask") == 0) {
      upload_umask = strtol(q,NULL,8);
    } else if(strcasecmp(p, "serverumask") == 0) {
      system_umask = strtol(q,NULL,8);
    } else if(strcasecmp(p, "daemonize") == 0) {
      daemonize = get_boolean(q);
    } else if(strcasecmp(p, "debug") == 0) {
      dbug = get_boolean(q);
    } else if(strcasecmp(p, "restricted") == 0) {
      priv_mode = get_boolean(q);
    } else if(strcasecmp(p, "homedir_restricted") == 0) {
      homedir_restricted = get_boolean(q);
    } else if(strcasecmp(p, "grabcommand") == 0) {
      grab_enabled = get_boolean(q);
    } else if(strcasecmp(p, "vercommand") == 0) {
      ver_enabled = get_boolean(q);
    } else if(strcasecmp(p, "permit_passwordless_owners") == 0) {
      permit_passwordless_owners = get_boolean(q);
    } else if(strcasecmp(p, "reverse_name") == 0) {
      no_unnamed = get_boolean(q);
    } else if(strcasecmp(p, "use_prebuild_dirlists") == 0) {
      use_prebuild_dirlists = get_boolean(q);
    } else if(strcasecmp(p, "use_access_files") == 0) {
      use_access_files = get_boolean(q);
    } else if(strcasecmp(p, "use_directory_mtime") == 0) {
      use_directory_mtime = get_boolean(q);
    } else if(strcasecmp(p, "read_only") == 0) {
      read_only = get_boolean(q);
    } else {
      fprintf(stderr, "Invalid command (%s) in config file, skipping.\n", p);
    }
  }
  fclose(fp);
}

void load_configuration (const char *config_file)
{
#ifdef LAMERPACK
    return;
#endif
  if(config_file == NULL) return;

  /* destroy_configuration(); */
  /* destroying iptab should be enough */
  if(iptab)
  {
      free_ip_table(iptab);
      iptab = NULL;
  }

  read_configuration(config_file);
}

void destroy_configuration (void)
{
    if(readme_file) free(readme_file);
    if(home_dir) free(home_dir);
    if(logname) free(logname);
    if(tlogname) free(tlogname);
    if(pidlogname) free(pidlogname);
    if(tmp_dir) free(tmp_dir);
    if(dumpname) free(dumpname);
    if(iptab) free_ip_table(iptab);
    if(listen_on) free(listen_on);

    readme_file = home_dir = logname = tmp_dir = dumpname = NULL;
    iptab = NULL;
    pidlogname = NULL;
    listen_on = NULL;
}
