#include "tweak.h"
#include "server_def.h"
#include "s_extern.h"

/****************************************************************************
 * Routine to return a 16-bit key with random number.
 ****************************************************************************/


/* The following algorithm is recommended by Numerical Recipies. */
/* Best, but needs floating point division. */
unsigned short gen_next_key PROTO0 ((void))
{
        unsigned short ulRandom = ((float)(0xffff)*rand()/(RAND_MAX+1.0f));
	return(ulRandom);
}

#if  0
unsigned short gen_next_key PROTO0((void))
{
  unsigned long k;
  
  k = random();
  k = k ^ (k >> 8) ^ (k >> 16) ^ (k << 8);
  
  return k;
}
#endif
