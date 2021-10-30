#ifndef FSP_CO_EXTERN_H_
#define FSP_CO_EXTERN_H_ 1

/* common routines for both server and clients */

/* udp_io.c */
int _x_udp (const char *, unsigned short *);
int _x_adr (const char *, int, struct sockaddr_in *);
int _x_select (fd_set *, long);

/* getsl.c */
char *getsl(char *s, int l);

#endif /* FSP_CO_EXTERN_H_ */
