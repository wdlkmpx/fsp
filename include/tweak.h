#ifndef _FSP_TWEAK_H_
#define _FSP_TWEAK_H_ 1
#include "config.h"

#ifndef HAVE_FSEEKO
/* fallback to old fseek if no fseeko is available */
#define fseeko fseek
#endif

#ifdef STAT_MACROS_BROKEN
#define S_ISREG(mode) ((mode) & S_IFREG)
#define S_ISDIR(mode) ((mode) & S_IFDIR)
#endif

#define FSP_STAT stat

#define fexist(A) (!access(A,F_OK))
#define touch(A) close(open(A,O_CREAT,0600))

#ifdef min
#undef min
#endif
#define min(x,y)  ((x) < (y) ? (x) : (y))

#if defined(HAVE_DIRENT_H)
#define HAVE_STRUCT_DIRENT 1
#else
#undef HAVE_STRUCT_DIRENT
#endif

#ifdef HAVE_TZFILE_H
#include <tzfile.h>
#endif
#ifndef SECSPERDAY
#define SECSPERDAY (long)60*60*24
#endif
#ifndef DAYSPERNYEAR
#define DAYSPERNYEAR 365
#endif

#if defined(HAVE_D_INO) && !defined(HAVE_D_FILENO)
#define d_fileno d_ino
#else
#if !defined(HAVE_D_INO) && defined(HAVE_D_FILENO)
#define d_ino d_fileno
#endif
#endif

#if !defined(BYTE)
  #if SIZEOF_CHAR == 1
    #define BYTE char
  #elif SIZEOF_VOID == 1
    #define BYTE void
  #else
    #error "Need 1 byte wide type"
  #endif
#endif
/****************************************************************************
*  Macros to read and write multi-byte fields from the message header.
****************************************************************************/

#if SIZEOF_SHORT == 2
#define WORD_TYPE_2 unsigned short
#else
#if SIZEOF_UNSIGNED == 2
#define WORD_TYPE_2 unsigned
#endif
#endif

#if SIZEOF_LONG == 4
#define WORD_TYPE_4 unsigned long
#else
#if SIZEOF_UNSIGNED == 4
#define WORD_TYPE_4 unsigned
#endif
#endif

#ifdef WORD_TYPE_4
/* there is an integer type of size 4 */
#define BB_READ4(V) ntohl(*(const WORD_TYPE_4 *)(V))
#define BB_WRITE4(V,A) *(WORD_TYPE_4 *)(V) = htonl(A)
#else
/* there is no integer type of size 4 */
#define BB_READ4(V) ((((V)[0] << 24) & 0xff000000) + \
		     (((V)[1] << 16) & 0x00ff0000) + \
		     (((V)[2] <<  8) & 0x0000ff00) + \
		     (((V)[3]      ) & 0x000000ff))

#define BB_WRITE4(V,A) ((V)[0] = ((A) >> 24) & 0xff, \
			(V)[1] = ((A) >> 16) & 0xff, \
			(V)[2] = ((A) >>  8) & 0xff, \
			(V)[3] = ((A)      ) & 0xff)
#endif

#ifdef WORD_TYPE_2
/* there is an integer type of size 2 */
#define BB_READ2(V) ntohs(*(WORD_TYPE_2 *)(V))
#define BB_WRITE2(V,A) *(WORD_TYPE_2 *)(V) = htons(A)
#else
/* there is no integer type of size 2 */
#define BB_READ2(V) ((((V)[0] <<  8) & 0xff00) + \
		     (((V)[1]      ) & 0x00ff))

#define BB_WRITE2(V,A) ((V)[0] = ((A) >>  8) & 0xff, \
			(V)[1] = ((A)      ) & 0xff)
#endif

#include "proto.h"

#endif /* _FSP_TWEAK_H_ */
