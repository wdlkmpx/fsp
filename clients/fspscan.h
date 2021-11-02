#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>

/****************************************************************************
*  UBUF is the structure of message exchanged between server and clients.
*
*    The 'buf' part of the buffer is variable lenght up to max of 1024.
*    The 'key' field is used by the server for sequence identification.
*    The 'seq' field is used by the client for sequence identification.
*
*  Client's message to server contain a key value that is the same as the
*  key value of the previous message received from the server.  Similarly,
*  the server's message to client contains a seq value that is the same
*  as the seq value of the previous message from the client.
*
*  The buf field is logically partitioned into two parts by the len field.
*  The len field indicate the size of the first part of the buffer starting
*  at buf[0].  The rest of the buffer is the second field.  In some cases
*  both fields can contain information.
*
****************************************************************************/

#define UBUF_HSIZE 12                           /* 12 bytes for the header */
#define UBUF_SPACE 1024			        /* maximum payload.        */

typedef struct UBUF {            char   cmd;  /* message code.             */
                        unsigned char   sum;  /* message checksum.         */
                        unsigned short  key;  /* message key.              */
                        unsigned short  seq;  /* message sequence number.  */
                        unsigned short  len;  /* number of bytes in buf 1. */
                        unsigned long   pos;  /* location in the file.     */

                        char   buf[UBUF_SPACE];
                    } UBUF;

/* definition of cmd */

#define CC_VERSION	0x10	/* return server's version string.	*/
#define CC_ERR          0x40    /* error response from server.          */
#define CC_GET_DIR      0x41    /* get a directory listing.             */
#define CC_GET_FILE     0x42    /* get a file.                          */
#define CC_UP_LOAD      0x43    /* open a file for writing.             */
#define CC_INSTALL      0x44    /* close a file opened for writing.     */
#define CC_DEL_FILE     0x45    /* delete a file.                       */
#define CC_DEL_DIR      0x46    /* delete a directory.                  */
#define CC_GET_PRO      0x47    /* get directory protection.            */
#define CC_SET_PRO      0x48    /* set directory protection.            */
#define CC_MAKE_DIR     0x49    /* create a directory.                  */
#define CC_BYE          0x4A    /* finish a session.                    */

#define NULLP ((void *) 0)

UBUF *client_interact(unsigned cmd, unsigned long pos, unsigned l1, unsigned char *p1,unsigned l2, unsigned char *p2);
void client_intr(int);
void init_client(char *host,int port,int myport);
int _x_select(fd_set *rf, long tt);
int _x_udp(int *port);
int _x_adr(char *host,int port,struct sockaddr_in *his);
