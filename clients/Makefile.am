## Process this file with automake to produce Makefile.in

bin_PROGRAMS = fspcmd

LDADD = -L../common -L../bsd_src -L../client -lclient -lcommon -lbsdfsp $(EX_LIBS)

fspcmd_SOURCES= \
	fspcmd.c \
	fspcmd_0misc.c \
	fspcmd_du.c \
	fspcmd_find.c \
	fspcmd_get.c \
	fspcmd_host.c \
	fspcmd_hostcmd.c \
	fspcmd_mv.c \
	fspcmd_put.c \
	fspcmd_setup.c \
	fspcmd_ver.c \
	fspprof.l
#	fspsites.l

noinst_HEADERS=fspcmd_0misc.h fspcmd_host.h

AM_CFLAGS = -DSYSCONFDIR="\"@sysconfdir@\"" $(AM_CFLAGS)

	
