#ifndef _FSP_CO_EXTERN_H_
#define _FSP_CO_EXTERN_H_ 1

#ifndef HAVE_STRDUP
/* strdup.c */
char *strdup PROTO0((char *));
#endif

#ifndef HAVE_RANDOM
/* random.c */
void srandom PROTO0((unsigned int));
char *initstate PROTO0((unsigned int, char *, int));
char *setstate PROTO0((char *));
long random PROTO0((void));
#endif

/* udp_io.c */
int _x_udp PROTO0((unsigned short *));
int _x_adr PROTO0((const char *, int, struct sockaddr_in *));
int _x_select PROTO0((fd_set *, long));

/* getsl.c */
char *getsl(char *s, int l);

#endif /* _FSP_CO_EXTERN_H_ */
