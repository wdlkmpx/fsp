## Process this file with automake to produce Makefile.in

bin_PROGRAMS=fspd
if MAINTAINER_MODE
noinst_PROGRAMS=cachecheck
endif

fspd_SOURCES=file.c host.c main.c conf.c filecache.c server.c fifocache.c \
	     log.c iprange.c acl.c
fspd_CFLAGS=-DSYSCONFDIR="\"@sysconfdir@\"" $(AM_CFLAGS)
fspd_LDADD=-L../common -lcommon

cachecheck_SOURCES=fifocache.c cachecheck.c

noinst_HEADERS=fifocache.h
