#ifndef _FSP_CLIENT_CONF_H_
#define _FSP_CLIENT_CONF_H_ 1

/****************************************************************************
 * Set this to point to the system wide default .fsp_prof file.             *
 * This file is used by fhostcmd to semi-automate the setting of            *
 * environment variable to ease the use of fsp                              *
 * It is only checked if neither ./.fsp_prof nor ~/.fsp_prof exist          *
 ****************************************************************************/
#define FSPRC SYSCONFDIR"/fsp_prof"

/****************************************************************************
 * The basename of the local startup file                                   *
 ****************************************************************************/
#define FSPPROF ".fsp_prof"

/****************************************************************************
 * Define the CLIENT_TIMEOUT if you want the client programs to time out
 * and abort after a certain period. Period is settable via an environment
 * variable FSP_TIMEOUT. See the fsp_env(7) for details
 ****************************************************************************/

/****************************************************************************
 * Define the following if you want fhostcmd to attempt to perform name     *
 * lookup on numeric host and numeric lookup on named hosts                 *
 ****************************************************************************/
#define HOST_LOOKUP 1

/****************************************************************************
 * The following code tries to set the file locking mechanism to the one    *
 * best suited for your system.  This should only be changed if the auto    *
 * configuration code fails and it doesn't compile.  That sort of bug       *
 * should also be immediately reported to the maintainers listed in the     *
 * INFO file                                                                *
 ****************************************************************************/
#define KEY_PREFIX "/tmp/.FSPL"

/* find the best locking method, defines one of USE_SHAREMEM_AND_LOCKF,
 * USE_FLOCK,USE_LOCKF,NOLOCKING 1 */
#if defined(HAVE_SHMEM) && defined(HAVE_SEMOP)
     #define USE_SHAREMEM_AND_SEMOP 1
#else     
  #if defined(HAVE_SHMEM) && defined(HAVE_LOCKF)
       #define USE_SHAREMEM_AND_LOCKF 1
  #else
       #ifdef HAVE_LOCKF
          #define USE_LOCKF 1
       #else
          #ifdef HAVE_FLOCK
             #define USE_FLOCK 1
          #else
             #define NOLOCKING
          #endif
       #endif
  #endif
#endif /* locking story */

#endif /* _FSP_CLIENT_CONF_H_ */
