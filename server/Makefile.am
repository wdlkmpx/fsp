## Process this file with automake to produce Makefile.in

bin_PROGRAMS=fspd

fspd_SOURCES=file.c host.c main.c conf.c server.c fifocache.c \
	     log.c iprange.c acl.c path.c random.c pidfile.c
fspd_CFLAGS=-DSYSCONFDIR="\"@sysconfdir@\"" $(AM_CFLAGS)
fspd_LDADD=-L../common -lcommon

noinst_HEADERS=fifocache.h pidfile.h
