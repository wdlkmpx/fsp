#ifndef _FSP_C_EXTERN_H_
#define _FSP_C_EXTERN_H_ 1

/* lib.c */
extern int client_trace;
extern unsigned long udp_sent_time,target_delay,target_maxdelay;
UBUF *client_interact PROTO0((unsigned char, unsigned long, unsigned int,
				     unsigned const char *, unsigned int,
				     unsigned const char *));
void init_client PROTO0((const char *, unsigned short, unsigned short));
int client_done PROTO0((void));

/* lock.c */
extern int key_persists;
unsigned short client_get_key PROTO0((void));
void client_set_key PROTO0((unsigned short));
void client_destroy_key PROTO0((void));
void client_init_key PROTO0((unsigned long, unsigned long,
				    unsigned short));

/* util.c */
extern const char *env_dir,*env_passwd,*env_local_dir,*env_port,*env_myport,*env_host,*env_listen_on;
extern unsigned int env_timeout;
extern unsigned short client_buf_len,client_net_len;
char *util_abs_path PROTO0((const char *));
char *util_getwd PROTO0((char *));
int util_download PROTO0((char *, FILE *, unsigned long));
int util_grab_file PROTO0((char *, FILE *, unsigned long));
int util_upload PROTO0((char *, FILE *, time_t));
void env_client PROTO0((void));
RDIR *util_opendir PROTO0((char *));
void util_closedir PROTO0((RDIR *));
rdirent *util_readdir PROTO0((RDIR *));
int util_stat PROTO0((char *, struct stat *));
int util_cd PROTO0((char *));
int util_cd2 PROTO0((char *));
void util_process_file PROTO0((char *, int,
			void (*)PROTO0((char *,struct stat *,int,int)),
			int (*)PROTO0((char *,struct stat *,u_long *)),
			void (*)PROTO0((char *,int,u_long,int)),
			int));

#endif /* _FSP_C_EXTERN_H_ */
