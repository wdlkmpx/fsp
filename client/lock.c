    /*********************************************************************\
    *  Copyright (c) 1991 by Wen-King Su (wen-king@vlsi.cs.caltech.edu)   *
    *                                                                     *
    *  You may copy or modify this file in any manner you wish, provided  *
    *  that this notice is always included, and that you hold the author  *
    *  harmless for any loss or damage resulting from the installation or *
    *  use of this software.                                              *
    \*********************************************************************/

#include "tweak.h"
#ifdef STDC_HEADERS
#include <stdlib.h>
#endif
#include "client_def.h"
#include "c_extern.h"
#include "my-string.h"

#ifndef NOLOCKING
static char key_string[sizeof(KEY_PREFIX)+32];

static char code_str[] =
    "0123456789:_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static void make_key_string(	unsigned long server_addr,
			   	unsigned long server_port)
{
  unsigned long v1, v2;
  char *p;

  strcpy(key_string,KEY_PREFIX);
  for(p = key_string; *p; p++);
  v1 = server_addr;
  v2 = server_port;

  *p++ = code_str[v1 & 0x3f]; v1 >>= 6;
  *p++ = code_str[v1 & 0x3f]; v1 >>= 6;
  *p++ = code_str[v1 & 0x3f]; v1 >>= 6;
  v1 = v1 | (v2 << (32-3*6));
  *p++ = code_str[v1 & 0x3f]; v1 >>= 6;
  *p++ = code_str[v1 & 0x3f]; v1 >>= 6;
  *p++ = code_str[v1 & 0x3f]; v1 >>= 6;
  *p++ = code_str[v1 & 0x3f]; v1 >>= 6;
  *p++ = code_str[v1 & 0x3f]; v1 >>= 6;
  *p   = 0;
}
#endif

/********************************************************************/
/******* For those systems that has flock function call *************/
/********************************************************************/
#ifdef USE_FLOCK

#include <sys/file.h>

int key_persists = 1;
static unsigned int lock_fd;
static unsigned short okey;

unsigned short client_get_key PROTO0((void))
{
  if(flock(lock_fd,LOCK_EX) == -1) {
    perror("flock");
    exit(1);
  }
  if(read(lock_fd,&okey,sizeof(okey)) == -1) {
    perror("read"); exit(1);
  }
  if(lseek(lock_fd,0L,0) == -1) {
    perror("seek");
    exit(1);
  }
  return(okey);
}

void client_set_key PROTO1(unsigned short, key)
{
  if(write(lock_fd,&key,sizeof(key)) == -1) {
    perror("write");
    exit(1);
  }
  if(lseek(lock_fd,0L,0) == -1) {
    perror("seek");
    exit(1);
  }
  if(flock(lock_fd,LOCK_UN) == -1) {
    perror("unflock");
    exit(1);
  }
}

void client_init_key PROTO3(unsigned long, server_addr,
			    unsigned long, server_port,
			    unsigned short, key)
{
  unsigned long omask;
  okey = key;

  make_key_string(server_addr,server_port);

  omask = umask(0);
  lock_fd = open(key_string,O_RDWR|O_CREAT,0666);
  umask(omask);
}

void client_destroy_key(void)
{
    (void)close(lock_fd);
    unlink(key_string);
}
#endif
/********************************************************************/
/******* For those systems that has lockf function call *************/
/********************************************************************/
#ifdef USE_LOCKF

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

int key_persists = 1;
static unsigned int lock_fd;
static unsigned short okey;

unsigned short client_get_key PROTO0((void))
{
  if(lockf(lock_fd,F_LOCK,sizeof(okey)) == -1) {
    perror("lockf");
    exit(1);
  }
  if(read(lock_fd,&okey,sizeof(okey)) == -1) {
    perror("read");
    exit(1);
  }
  if(lseek(lock_fd,0L,0) == -1) {
    perror("seek");
    exit(1);
  }
  return(okey);
}

void client_set_key PROTO1(unsigned short, key)
{
  if(write(lock_fd,&key,sizeof(key)) == -1) {
    perror("write");
    exit(1);
  }
  if(lseek(lock_fd,0L,0) == -1) {
    perror("seek");
    exit(1);
  }
  if(lockf(lock_fd,F_ULOCK,sizeof(key)) == -1) {
    perror("unlockf");
    exit(1);
  }
}

void client_init_key PROTO3(unsigned long, server_addr,
			    unsigned long, server_port,
			    unsigned short, key)
{
  unsigned long omask;
  okey = key;

  make_key_string(server_addr,server_port);

  omask = umask(0);
  lock_fd = open(key_string,O_RDWR|O_CREAT,0666);
  umask(omask);
}

void client_destroy_key(void)
{
    (void)close(lock_fd);
    unlink(key_string);
}
#endif
/********************************************************************/
/******* For those systems that has SysV shared memory + lockf ******/
/********************************************************************/
#ifdef USE_SHAREMEM_AND_LOCKF

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/ipc.h>
#include <sys/shm.h>

int key_persists = 0;
static unsigned short *share_key;
static unsigned int lock_fd;
static int   lock_shm;

unsigned short client_get_key (void)
{
  if(lockf(lock_fd,F_LOCK,2) == -1) {
    perror("lockf");
    exit(1);
  }
  return(*share_key);
}

void client_set_key (unsigned short key)
{
  *share_key = key;
  if(lockf(lock_fd,F_ULOCK,2) == -1) {
    perror("unlockf");
    exit(1);
  }
}

void client_init_key (unsigned long server_addr,
			    unsigned long server_port,
			    unsigned short key)
{
  unsigned long omask;
  key_t lock_key;

  make_key_string(server_addr,server_port);

  omask = umask(0);
  lock_fd = open(key_string,O_RDWR|O_CREAT,0666);
  umask(omask);

  if((lock_key = ftok(key_string,238)) == -1) {
    perror("ftok");
    exit(1);
  }
  if((lock_shm = shmget(lock_key,sizeof(short),IPC_CREAT|0666)) == -1) {
    perror("shmget");
    exit(1);
  }
  if(!(share_key = (unsigned short *) shmat(lock_shm,(char*)0,0))) {
    perror("shmat");
    exit(1);
  }
  *share_key = key;
}

void client_destroy_key(void)
{
    (void)close(lock_fd);
    if (shmdt((char *)share_key) < 0)
    {
	perror("shmdt");
	exit(1);
    }
    if (shmctl(lock_shm,IPC_RMID,NULL) < 0)
    {
	perror("shmctl");
	exit(1);
    }
    unlink(key_string);
}
#endif
/********************************************************************/
/******* For those who does not want to use locking *****************/
/********************************************************************/
#ifdef NOLOCKING

int key_persists = 0;
static unsigned short okey;

unsigned short client_get_key (void)
{
  return(okey);
}

void client_set_key (unsigned short key)
{
  okey = key;
}

void client_init_key (unsigned long server_addr,
			    unsigned long server_port,
			    unsigned short key)
{
  okey = key;
}

void client_destroy_key(void)
{
    return;
}
#endif
/********************************************************************/
/********************************************************************/
/********************************************************************/
