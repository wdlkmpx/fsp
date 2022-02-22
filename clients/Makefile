include ../config.mk

APPS  = fspcmd fspscan
APPS_INST  =  fspcmd fspscan

# ls *.c | sed "s%\.c%.o%g" | tr "\n" " " >> Makefile

fspcmd_cflags = ${CFLAGS}
fspcmd_ldflags = ${LDFLAGS} ${LIBS}
fspcmd_objs =  fspcmd.o fspcmd_0misc.o fspcmd_du.o fspcmd_find.o fspcmd_get.o fspcmd_mv.o fspcmd_put.o fspcmd_ver.o
fspscan_cflags = ${CFLAGS}
fspscan_ldflags = ${LDFLAGS} ${LIBS}
fspscan_objs = fspscan.c

LDFLAGS +=  -L../common -L../bsd_src -L../client -lclient -lcommon -lbsdfsp $(EX_LIBS)
#CPPFLAGS += -I..
CFLAGS += -I..
LIBS   += 

all: $(APPS)

fspcmd: $(fspcmd_objs)
	$(CC) $(fspcmd_cflags) $(fspcmd_objs) -o $@ $(fspcmd_ldflags)

fspscan: $(fspscan_objs)
	$(CC) $(fspscan_cflags) $(fspscan_objs) -o $@ $(fspscan_ldflags)

strip: $(APPS)
	$(STRIP) $(APPS)

distclean: clean

clean:
	rm -f *.o *~ $(APPS)

install: all 
	mkdir -p $(DESTDIR)$(bindir)
	install -c $(APPS_INST) $(DESTDIR)$(bindir)

install-strip: strip install

uninstall: 
	rm -f $(DESTDIR)$(bindir)/fspcmd
	rm -f $(DESTDIR)$(bindir)/fspscan

