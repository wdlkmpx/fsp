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


static int myfd;
static struct sockaddr_in server_addr;
static unsigned short myseq = 0;
static unsigned short key;

int client_trace = 0;
int client_intr_state = 0;
unsigned long target_delay = DEFAULT_DELAY;	/* expected max delay	 */
unsigned long busy_delay   = DEFAULT_DELAY;	/* busy retransmit timer */
unsigned long idle_delay   = DEFAULT_DELAY;	/* idle retransmit timer */
unsigned long udp_sent_time;

UBUF *client_interact PROTO6(unsigned char, cmd, unsigned long, pos,
			     unsigned int, l1, unsigned const char *, p1,
			     unsigned int, l2, unsigned const char *, p2)
{
  struct sockaddr_in from;
  UBUF sbuf;
  static UBUF rbuf;
  unsigned char *s, *t, *d, seq0, seq1;
  unsigned u, n, sum, mlen, rlen;
  fd_set mask;
  int retval, bytes, retry_send, retry_recv;
  unsigned long w_delay;

  FD_ZERO(&mask);
  sbuf.cmd = cmd;

#ifdef DEBUG
  printf("sbuf.cmd = %u\n",sbuf.cmd);
#endif

  BB_WRITE2(sbuf.bb_len,l1);
  BB_WRITE4(sbuf.bb_pos,pos);
  
  client_intr_state = 1;
  
  for(u = l1, d = (unsigned char *) sbuf.buf; u--; *d++ = *p1++);
  for(u = l2; u--; *d++ = *p2++);
  mlen = d - (unsigned char *) &sbuf;
  
  key = client_get_key();
  
  for(retry_send = 0; ; retry_send++) {
    BB_WRITE2(sbuf.bb_key,key);
    sbuf.bb_seq[0] = seq0 = (myseq >> 8) & 0xff;
    sbuf.bb_seq[1] = seq1 = (myseq & 0xfc) | (retry_send & 0x0003);
    sbuf.sum = 0;
    
    for(t = (unsigned char *) &sbuf, sum = n = mlen; n--; sum += *t++);
    sbuf.sum = sum + (sum >> 8);
      
    switch(retry_send) {	/* adaptive retry delay adjustments */
      case  0:
        busy_delay = (target_delay+(busy_delay<<3)-busy_delay)>>3;
	w_delay = busy_delay;
	break;
      case  1:
	busy_delay = busy_delay * 3 / 2;
	w_delay = busy_delay;
	if(client_trace) write(2,"R",1); 
	break;
	  
      default:
#ifdef CLIENT_TIMEOUT
	if (!pos && retry_send >= env_timeout ) {
	  fprintf(stderr, "\rRemote server not responding.\n");
	  exit(1);
	}
#endif
	if(idle_delay < 3*60*1000) idle_delay = idle_delay * 3 / 2;
	w_delay = idle_delay;
	if(client_trace) write(2,"I",1);
	break;
    }
      
    if(sendto(myfd,(const char*)&sbuf,mlen,0,(struct sockaddr *)&server_addr,
	      sizeof(server_addr)) == -1) {
      perror("sendto");
      exit(1);
    }
    udp_sent_time = time((time_t *) 0);
    FD_SET(myfd,&mask);
      
    for(retry_recv = 0; ; retry_recv++) {
      if(retry_recv && client_trace) write(2,"E",1);
      retval = _x_select(&mask, w_delay);
      if((retval == -1) && (errno == EINTR)) continue;
      if(retval == 1) { /* an incoming message is waiting */
	bytes = sizeof(from);
	if((bytes = recvfrom(myfd,(char*)&rbuf,sizeof(rbuf),0,
			     (struct sockaddr *)&from, &bytes)) < UBUF_HSIZE)
	  continue;
	      
	s = (unsigned char *) &rbuf;
	d = s + bytes;
	u = rbuf.sum; rbuf.sum = 0;
	for(t = s, sum = 0; t < d; sum += *t++);
	sum = (sum + (sum >> 8)) & 0xff;
	if(sum != u) continue;  /* wrong check sum */
	      
	rlen = BB_READ2(rbuf.bb_len);
	      
	if( (rbuf.bb_seq[0] ^ seq0) ||
	   ((rbuf.bb_seq[1] ^ seq1)&0xfc)) continue;  /* wrong seq # */
	if((int) (rlen+UBUF_HSIZE)  > bytes) continue;  /* truncated.  */
	      
	myseq = (myseq + 0x0004) & 0xfffc;  /* seq for next request */
	key = BB_READ2(rbuf.bb_key); /* key for next request */
	      
	client_set_key(key);
	      
	if(client_intr_state == 2) {
	  if(!key_persists) client_done();
	  exit(1);
	}

#ifdef DEBUG
  printf("rbuf.cmd = %u\n",rbuf.cmd);
#endif
      
	return(&rbuf);
	
      } else break;   /* go back to re-transmit buffer again */
    }
  }
}

static RETSIGTYPE client_intr PROTO1(int, signum)
{
  switch(client_intr_state) {
    case 0: exit(2);
    case 1: client_intr_state = 2; break;
    case 2: exit(3);
  }
#ifndef RELIABLE_SIGNALS  
  signal(SIGINT,client_intr);
#endif  
}

void init_client PROTO3(const char *, host, unsigned short, port, unsigned short, myport)
{
  busy_delay = idle_delay = target_delay;
  
  if((myfd = _x_udp(&myport)) == -1) {
    perror("socket open");
    exit(1);
  }
  
  if(_x_adr(host,port,&server_addr) == -1) {
    perror("server addr");
    exit(1);
  } 
  
  client_init_key(server_addr.sin_addr.s_addr,port,getpid());
  signal(SIGINT,client_intr);
}

int client_done PROTO0((void))
{
  (void) client_interact(CC_BYE, 0L, 0, (unsigned char *)NULLP, 0,
			 (unsigned char *)NULLP);
  return(0);
}

