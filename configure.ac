dnl Process this file with autoconf to produce a working configure script.
dnl tested with autoconf 2.57
AC_INIT(fsp,2.8.1b19,hsn@netmag.cz)
AC_CONFIG_SRCDIR(server/main.c)
AM_INIT_AUTOMAKE([dist-bzip2])
AM_MAINTAINER_MODE
AC_CONFIG_HEADERS(include/config.h)
AC_CANONICAL_HOST
AC_MSG_CHECKING([if bulding for DYNIX/ptx])
dnl check for dynix/ptx
if test "x$build_vendor" = "xsequent"; then
EX_LIBS="-lsocket -linet -lnsl -lseq"
CC='cc'
GCC=
AC_MSG_RESULT(yes)
else
AC_MSG_RESULT(no)
fi
AC_MSG_CHECKING([if building for SunOS 5])
if test "x$build_os" = "xsolaris2" ; then
EX_LIBS="-lsocket -lnsl"
AC_SUBST(EX_LIBS)
AC_MSG_RESULT(yes)
else
AC_MSG_RESULT(no)
fi
AC_MSG_CHECKING([if building for Cygwin])
case $host_os in
           *cygwin* ) AC_DEFINE(OS_CYGWIN,1,[Define if you are building for cygwin]) AC_MSG_RESULT(yes);;
                  * ) AC_MSG_RESULT(no);;
esac		  
AC_PROG_RANLIB
AM_PROG_LEX
AC_PROG_LN_S
AC_PROG_CC
dnl If we're using gcc, enable some warnings
AC_CACHE_CHECK(for additional compiler options, ac_cv_prog_gcc_flags, [
ac_cv_prog_gcc_flags=""
if test "$GCC" = yes
then
  echo "void dummy(void);" >configure-dummy.c
  echo "void dummy(void) {}" >>configure-dummy.c
  addopts=
  case `$CC --version` in
      *2.7*) ;;
      *) addopts="$addopts -Wpointer-arith" ;;
  esac
  for i in -Wall -W -Wstrict-prototypes -Wmissing-prototypes -Wshadow \
      -Wbad-function-cast -Wcast-qual -Wcast-align -Wwrite-strings \
      -Waggregate-return -Wmissing-declarations \
      -Wmissing-format-attribute -Wnested-externs \
      -ggdb -fno-common -Wchar-subscripts -Wcomment \
      -Wimplicit -Wsequence-point -Wreturn-type \
      -Wfloat-equal $addopts \
      -Wno-system-headers -Wredundant-decls \
      -Wmissing-noreturn -pedantic \
      -Wlong-long -Wundef -Winline \
      -Wno-unused-parameter
#     -Wunreachable-code -Wconversion 
  do
       if $CC $i $ac_cv_prog_gcc_flags -c configure-dummy.c >/dev/null 2>&1 
       then ac_cv_prog_gcc_flags="$ac_cv_prog_gcc_flags $i"
       else echo "ignoring $i"
       fi
  done
fi])
rm -f configure-dummy.c configure-dummy.o
CFLAGS="$CFLAGS $ac_cv_prog_gcc_flags"
AC_SUBST(CFLAGS)
AC_AIX
AC_C_PROTOTYPES
AC_C_CONST
AC_C_VOLATILE
AC_PROG_CPP
AC_ISC_POSIX
AC_SYS_LARGEFILE
if test "${enable_maintainer_mode+set}" = set; then
   CFLAGS="-g3 $ac_cv_prog_gcc_flags"
AC_CHECK_LIB(efence,EF_Abort)   
fi
AC_MSG_CHECKING([if enable extra debug code])
AC_ARG_ENABLE(debug,[Enable client library debuging],AC_DEFINE(DEBUG,1,[Define if you want more debug messages]) AC_MSG_RESULT(YES),AC_MSG_RESULT(NO))
AC_HEADER_STDC
AC_CHECK_HEADERS(limits.h sys/syslimits.h errno.h)
AC_CHECK_HEADERS(string.h memory.h strings.h unistd.h sys/resource.h utime.h)
AC_HEADER_DIRENT
AC_HEADER_TIME
AC_CHECK_SIZEOF(unsigned)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(void)
AC_FUNC_FSEEKO
AC_CHECK_SIZEOF(off_t)
if test $ac_cv_sizeof_off_t -ge 8 -a "$ac_cv_func_fseeko" = yes; then
AC_DEFINE(NATIVE_LARGEFILES,1,[Define if we have 64bit off_t and fseeko])
AC_MSG_NOTICE([Native large file support enabled])
else
AC_MSG_NOTICE([No native 64 bit files support, fallback to fopen64])
AC_CHECK_FUNC(fopen64)
if test "$ac_cv_func_fopen64" = yes -a "$enable_largefile" = yes; then
  AC_MSG_ERROR([You have fopen64 but no native LFS support. This configuration is not currently supported. Report compiler version, os version and library version to Radim Kolar. Thank you for your cooperation.])
fi
fi
AC_TYPE_MODE_T
AC_TYPE_UID_T
AC_TYPE_SIZE_T
AC_TYPE_PID_T
AC_CHECK_TYPE(nlink_t, ,AC_DEFINE(nlink_t, int,[declare nlink_t as int if not found in <sys/types.h>]))
AC_CHECK_TYPE(rlim_t, ,AC_DEFINE(rlim_t, [unsigned int],[declare rlim_t as unsigned int if not found in <sys/resource.h>]),[#include <sys/resource.h>])
AC_TYPE_SIGNAL
AC_FUNC_CLOSEDIR_VOID
AC_CHECK_FUNCS(random strdup bcopy flock lockf getrlimit fork setsid)
AC_CACHE_CHECK(for working shmget ,fsp_cv_func_shmget,
[LOCKPATH="conftestlock"
AC_TRY_RUN([
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define LOCKPATH "$LOCKPATH"

main() {
  key_t ky;
  int   sh, rv, fd;

  fd = open(LOCKPATH, O_RDWR | O_CREAT, 0666);
  if (fd == -1) exit(1);
  rv = 0;
  ky = ftok(LOCKPATH, 2001);
  if (ky == (key_t) -1) rv = 1;
  else {
    struct shmid_ds foo;
    sh = shmget(ky, 1, IPC_CREAT | 0666);
    if (sh == -1) rv = 1;
    else shmctl(sh, IPC_RMID, &foo);
  }
  close(fd);
  exit(rv); }],fsp_cv_func_shmget="yes",fsp_cv_func_shmget="no")
rm -f $LOCKPATH conftest.c
])
if test "x$fsp_cv_func_shmget" = "xyes"; then 
            AC_DEFINE(HAVE_SHMEM,1,[Define the following if you have working shared memory])
fi
AC_CHECK_MEMBER(struct dirent.d_ino,AC_DEFINE(HAVE_D_INO,1,[define if you have d_ino in struct dirent]), AC_CHECK_MEMBER(struct direct.d_ino,AC_DEFINE(HAVE_D_INO,1,[do you have d_ino in struct direct]), ,[
#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#else
#ifdef HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#endif
])
,[#include <dirent.h>])
AC_CHECK_MEMBER(struct dirent.d_fileno,AC_DEFINE(HAVE_D_FILENO,1,[define if you have d_fileno in struct dirent]), AC_CHECK_MEMBER(struct direct.d_fileno,AC_DEFINE(HAVE_D_FILENO,1,[do you have d_fileno in struct direct]), ,[
#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#else
#ifdef HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#endif
])
,[#include <dirent.h>])
AC_HEADER_SYS_WAIT
AC_HEADER_STAT
AC_CHECK_HEADER(tzfile.h)
AC_CACHE_CHECK(for string.h declaring some functions incorrectly,fsp_cv_header_string_bogus,[
AC_TRY_LINK([#include <sys/types.h>
#include <string.h>],,
  fsp_cv_header_string_bogus="no",
  fsp_cv_header_string_bogus="yes"
)  
])
if test "x$fsp_cv_header_string_bogus" = "xyes"; then AC_DEFINE(STRING_H_BOGUS,1,[Define if your string library is broken])
fi
AC_FUNC_VPRINTF
AC_CACHE_CHECK(for reliable signals ,fsp_cv_sys_reliable_signals,[
AC_TRY_RUN(
[#include <stdio.h>
#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
volatile int test;
RETSIGTYPE sig_handler(sg)
int sg;
{ test=1;} 
main() {
 signal(SIGUSR1,sig_handler);
 test=0;
 kill(getpid(),SIGUSR1);
 sleep(1);
 if(test==0) { printf("Signals ARE BROKEN!\n");kill(0,SIGQUIT);}
 test=0;
 kill(getpid(),SIGUSR1);
 if(test==1) exit(0); else exit(1);
}],
fsp_cv_sys_reliable_signals="yes",fsp_cv_sys_reliable_signals="no")])
if test "x$fsp_cv_sys_reliable_signals" = "xyes"; then AC_DEFINE(RELIABLE_SIGNALS,1,[undefine If your signal handlers don't reinstall automatically])
fi
AC_CONFIG_FILES(Makefile bsd_src/Makefile common/Makefile client/Makefile server/Makefile clients/Makefile man/Makefile include/Makefile contrib/Makefile doc/Makefile tests/Makefile)
AC_OUTPUT
