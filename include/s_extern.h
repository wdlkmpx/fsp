#ifndef _FSP_S_EXTERN_H_
#define _FSP_S_EXTERN_H_ 1

/* conf.c, read and check configuration */
extern int daemonize,use_prebuild_dirlists,read_only,dbug;
extern int permit_passwordless_owners;
extern int use_access_files,use_directory_mtime;
extern int priv_mode,no_unnamed,logging,grab_enabled,ver_enabled;
extern int homedir_restricted;
extern uid_t run_uid;
extern gid_t run_gid;
extern unsigned int maxthcallowed;
extern unsigned short udp_port,packetsize;
extern time_t retry_timeout,session_timeout,stat_cache_timeout;
extern char *logname,*tlogname,*readme_file,*dumpname;
extern char *home_dir;
extern unsigned int dir_cache_limit, stat_cache_limit,fp_cache_limit;
extern char *tmp_dir;
extern mode_t upload_umask, system_umask;
void load_configuration PROTO1(const char *, conffile);
void destroy_configuration PROTO0((void));

/* file.c, file based disk i/o operations */
int init_caches PROTO0((void));
void shutdown_caches PROTO0((void));
void stat_caches PROTO1(FILE *,fp);
const char *validate_path PROTO0((char *, unsigned, PPATH *,DIRINFO **, int));
const char *server_get_dir PROTO0((DIRLISTING **,const DIRINFO *));
const char *server_del_file PROTO0((PPATH *, DIRINFO *));
const char *server_del_dir PROTO2(PPATH *, pp, DIRINFO *,di);
const char *server_make_dir PROTO0((PPATH *, unsigned long,DIRINFO **));
const char *server_get_file PROTO0((PPATH *, FILE **, unsigned long,
				     unsigned short,DIRINFO *));
const char *server_get_pro PROTO3(DIRINFO *, di, char *, result, const char *, acc);
const char *server_set_pro PROTO2(DIRINFO *,di, const char *, key);
const char *server_up_load PROTO0((char *, unsigned int, unsigned long, unsigned long,
				    unsigned short));
const char *server_install PROTO0((PPATH *, unsigned long, unsigned short,const char *,DIRINFO *,unsigned int,const char *));
const char *server_secure_file PROTO0((PPATH *, unsigned long,
					unsigned short,DIRINFO *));
const char *server_grab_file PROTO0((FILE **, unsigned long,
				      unsigned short));
const char *server_grab_done PROTO0((unsigned long, unsigned short));
const char *server_stat PROTO1(UBUF *, buf);
const char *server_rename PROTO0((char *,unsigned int,unsigned int));
void init_home_dir PROTO0((void));

/* filecache.c, open filehandles cache */

/* iprange.c IP range services */
extern IPrange *iptab;
const char *check_ip_table PROTO2(unsigned long, inet_num,IPrange *,table);
void free_ip_table PROTO1(IPrange *,table);
void add_ipline PROTO2(const char *, text, IPrange **, table);
void dump_iptab PROTO2(IPrange *,table,FILE *, fp);

/* host.c, DNS and IP host databases */
HTAB *find_host PROTO0((unsigned long));
int init_htab PROTO0((void));
int dump_htab PROTO1(FILE *,fn);

/* server.c, network server operations */
extern time_t cur_time;
extern int shutdowning;
RETSIGTYPE server_interrupt PROTO1(int, signum);
RETSIGTYPE server_dump PROTO1(int, signum);
int server_loop PROTO2(int,fd,time_t,timeout);
int server_reply PROTO0((struct sockaddr_in *, UBUF *, unsigned int, unsigned int));
void send_file PROTO0((struct sockaddr_in *, UBUF *, FILE *, unsigned int,
			      char *));

/* acl.c, security code */
void load_access_rights PROTO1(DIRINFO *,di);
void save_access_rights PROTO1(DIRINFO *,di);
const char * require_access_rights PROTO4(const DIRINFO *,di,unsigned char,rights,unsigned long, ip_addr, const char *, passwd);

/* main.c, startup and init code */

/* log.c */
extern int logfd;
extern int tlogfd;
void fsplogf PROTO0((void));
void fsplogs PROTO0((void));
#ifdef __STDC__
void fsploga(const char *fmt, ...);
#else
void fsploga(va_alist);
#endif
void xferlog(char direction, const char *filename,unsigned long filesize,const char *hostname);

#endif /* _FSP_S_EXTERN_H_ */
