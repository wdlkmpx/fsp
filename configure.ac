# Process this file with autoconf to produce a working configure script.

AC_PREREQ([2.59])
AC_INIT([fsp],[2.8.1b30],[])
AC_CONFIG_AUX_DIR([autoconf])
AC_CONFIG_MACRO_DIR([autoconf-m4])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
m4_ifdef([AM_MAINTAINER_MODE], [AM_MAINTAINER_MODE])

AC_CONFIG_HEADERS(include/config.h)
AC_ARG_WITH(lockprefix,AS_HELP_STRING([--with-lockprefix=path],[Set lock prefix path to (default /tmp/.FSPL)]))
#AC_ARG_WITH([locking],AS_HELP_STRING([--with-locking=none/semop/shmget/lockf/flock],[Set client locking type (default none)]))
AC_ARG_WITH([locking],AS_HELP_STRING([--with-locking=none/semop/lockf/flock],[Set client locking type (default none)]))
AC_CANONICAL_HOST
AC_MSG_CHECKING([if we are building lamer pack])
AC_ARG_ENABLE(lamerpack,AS_HELP_STRING([--enable-lamerpack],[Build fspd tuned for lamers (default no)]),AC_DEFINE(LAMERPACK,1,[Define if you want to build for lamer disabled ppl]) AC_MSG_RESULT(YES),AC_MSG_RESULT(NO))
AC_MSG_CHECKING([if enabling of extra debug code is requested])
AC_ARG_ENABLE(debug,AS_HELP_STRING([--enable-debug],[Enable client library debuging output (default no)]),AC_DEFINE(DEBUG,1,[Define if you want more debug messages]) AC_MSG_RESULT(yes),AC_MSG_RESULT(no))
AC_MSG_CHECKING([If client commands can time out])
AC_ARG_ENABLE(timeout,AS_HELP_STRING([--disable-timeout],[Disable timeouts in client commands (default enabled)]),,enable_timeout=yes)
if test "x${enable_timeout}" = xyes; then
   AC_DEFINE(CLIENT_TIMEOUT,1,[Define if you want client commands timeout])
   AC_MSG_RESULT(yes)
else
   AC_MSG_RESULT(no)
fi
AC_PROG_RANLIB
AC_PROG_CC

CFLAGS="$CFLAGS"
AC_SUBST(CFLAGS)
AC_SEARCH_LIBS([strerror],[cposix])

AC_CHECK_HEADERS(limits.h sys/syslimits.h errno.h signal.h sys/shm.h)
AC_CHECK_HEADERS(string.h strings.h unistd.h sys/resource.h utime.h)
AC_HEADER_DIRENT
AC_HEADER_TIME
AC_HEADER_SYS_WAIT
AC_HEADER_STAT
AC_SYS_LARGEFILE
AC_CHECK_FUNCS(fseeko)

AC_CHECK_SIZEOF(off_t)
if test $ac_cv_sizeof_off_t -ge 8 -a "x$ac_cv_func_fseeko" = xyes; then
	AC_DEFINE(NATIVE_LARGEFILES,1,[Define if we have 64bit off_t and fseeko])
	AC_MSG_NOTICE([Native 64bit large file support enabled])
else
	AC_MSG_NOTICE([No native large 64bit files support detected])
fi

AC_TYPE_MODE_T
AC_TYPE_UID_T
AC_TYPE_PID_T
AC_CHECK_TYPE(nlink_t, ,AC_DEFINE(nlink_t, int,[declare nlink_t as int if not found in <sys/types.h>]))

AC_CHECK_FUNCS(random srandomdev getrlimit fork setsid flock lockf semop shmget)

#AC_DEFINE(HAVE_D_INO,1,[define if you have d_fileno in struct dirent])
AC_CHECK_MEMBER(struct dirent.d_ino,AC_DEFINE(HAVE_D_INO,1,[do you have d_ino in struct dirent]), [], [#include <dirent.h>])
AC_CHECK_MEMBER(struct dirent.d_fileno,AC_DEFINE(HAVE_D_FILENO,1,[do you have d_fileno in struct dirent]), [], [#include <dirent.h>])

# signal.h
AC_DEFINE(RELIABLE_SIGNALS,1,[undefine If your signal handlers don not reinstall automatically])

#Parse user locking choice
if test "x$with_locking" = "xno" -o "x$with_locking" = "xnone" ; then
	AC_DEFINE(FSP_NOLOCKING,1,[Define if you do not want to use any locking])
	AC_MSG_NOTICE(locking disabled by user)
elif test "x$with_locking" = "xlockf" -a "x$ac_cv_func_lockf" = "xyes"; then
	AC_DEFINE(FSP_USE_LOCKF,1,[Define for lockf style locking])
elif test "x$with_locking" = "xsemop" -a "x$ac_cv_func_semop" = "xyes" -a "x$ac_cv_func_shmget" = "xyes";then
	AC_DEFINE(FSP_USE_SHAREMEM_AND_SEMOP,1,[Define for sharemem+semop locking])
	#Autodetect locking 
elif test "x$ac_cv_func_semop" = "xyes" -a "x$ac_cv_func_shmget" = "xyes"; then
	AC_DEFINE(FSP_USE_SHAREMEM_AND_SEMOP,1)
elif test "x$ac_cv_func_lockf" = "xyes"; then
	AC_DEFINE(FSP_USE_LOCKF,1,[Define for lockf style locking])
elif test "x$ac_cv_func_flock" = "xyes"; then
	AC_DEFINE(FSP_USE_FLOCK,1,[Define for flock style locking])
else
	AC_DEFINE(FSP_NOLOCKING,1,[Define if you do not want to use any locking])
	AC_MSG_NOTICE([no suitable locking method detected])
fi

#locking prefix
if test "x$with_lockprefix" != "xno" -a "x$with_lockprefix" != "xyes" -a "x$with_lockprefix" != "x" ; then
    AC_DEFINE_UNQUOTED(FSP_KEY_PREFIX,"$with_lockprefix",[Key locking prefix])
fi

AC_CONFIG_FILES(
	Makefile
	bsd_src/Makefile
	common/Makefile
	client/Makefile
	server/Makefile
	clients/Makefile
	man/Makefile
	include/Makefile
	doc/Makefile
	tests/Makefile
)
AC_OUTPUT
