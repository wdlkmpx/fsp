#ifndef _FSP_MY_STRING_H_
#define _FSP_MY_STRING_H_ 1

#if defined(STRING_H_BOGUS) && defined(STDC_HEADERS)
extern void     bzero(char *, int);
extern void    *memset(void *, int, size_t);
extern char    *strcpy(char *, const char *);
extern char    *strncpy(char *, const char *, size_t);
extern char    *strcat(char *, const char *);
extern int      strcmp(const char *, const char *);
extern int      strncmp(const char *, const char *, size_t);
extern char    *strchr(const char *, int);
extern char    *strerror(int);
#else

#if STDC_HEADERS || HAVE_STRING_H
#include <string.h>
/* An ANSI string.h and pre-ANSI memory.h might conflict.  */
#if !STDC_HEADERS && HAVE_MEMORY_H
#include <memory.h>
#endif /* not STDC_HEADERS and HAVE_MEMORY_H */

#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#endif

#ifndef HAVE_BCOPY
#define bcopy(s, d, n) memcpy((d), (s), (n))
#endif

#endif /* _FSP_MY_STRING_H_ */
