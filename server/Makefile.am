## Process this file with automake to produce Makefile.in

bin_PROGRAMS=fspd

fspd_SOURCES=file.c host.c main.c conf.c filecache.c server.c fifocache.c \
	     log.c iprange.c acl.c path.c random.c
fspd_CFLAGS=-DSYSCONFDIR="\"@sysconfdir@\"" $(AM_CFLAGS)
fspd_LDADD=-L../common -lcommon

noinst_HEADERS=fifocache.h
