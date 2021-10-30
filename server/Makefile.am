## Process this file with automake to produce Makefile.in

bin_PROGRAMS=fspd

fspd_SOURCES = \
	acl.c \
	conf.c \
	fifocache.c \
	file.c \
	host.c \
	iprange.c \
	log.c \
	main.c \
	path.c \
	pidfile.c \
	server.c

fspd_CFLAGS=-DSYSCONFDIR="\"@sysconfdir@\"" $(AM_CFLAGS)
fspd_LDADD=-L../common -lcommon $(EX_LIBS)

noinst_HEADERS=fifocache.h pidfile.h

