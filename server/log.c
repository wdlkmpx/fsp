   /*********************************************************************\
   *  Copyright (c) 2003 by Radim Kolar (hsn@cybermail.net)              *
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
#include "my-string.h"
#include <stdarg.h>

int logfd = -1;  /* logfile file descriptor */
int tlogfd = -1; /* transfer log file descriptor */

/****************************************************************************
 * A slightly better logging function.. It now takes a format string and    *
 * any number of args.                                                      *
 ****************************************************************************/

#define LOGBUFFER 1024+80
static char logbuf[LOGBUFFER];	/* buffer for log message */
static size_t logpos = 0;	/* current log message length */

/* append some text to the log message */
void fsploga(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    /* fmt = va_arg(args, char *); */
    vsprintf(logbuf + logpos, fmt, args);
    logpos += strlen(logbuf + logpos);
    va_end(args);
}

/* add a datestamp to begining of the log message, clear text in buffer */
void fsplogs PROTO0((void))
{

    int timelen;
    char *timestr;

    timestr = (char *)ctime(&cur_time);
    timelen = strlen(timestr) - 1; /* strip the CR */
    timestr[timelen] = '\0';
    strcpy(logbuf, timestr);
    logbuf[timelen] = ' ';
    logpos = timelen + 1;
}

/* flush logfile */
void fsplogf PROTO0((void))
{
  if(dbug) {
	     fwrite("[LOG] ",6,1,stdout);
	     fwrite(logbuf,logpos,1,stdout);
	     fflush(stdout);
	   }
   write(logfd, logbuf, logpos);
   logpos = 0;
}

/* wu ftpd xferlog subsystem */
static char tlogbuf[LOGBUFFER];	/* buffer for log message */

void xferlog(char direction, const char *filename,unsigned long filesize,const char *hostname)
{
    size_t pos=0,timelen;
    char *timestr;

    if(!tlogname) return; /* xfer logging is not enabled */

    /* current-time */
    timestr = (char *)ctime(&cur_time);
    timelen = strlen(timestr) - 1; /* strip the CR */
    timestr[timelen] = '\0';
    strcpy(tlogbuf, timestr);
    pos = timelen;
    /* transfer-time, remote-host, file-size, file-name, ... */
    pos+=sprintf(tlogbuf+pos," 1 %s %lu %s b * %c a fsp fsp 0 * c\n",hostname,filesize,filename,direction);
   write(tlogfd, tlogbuf, pos);
}
