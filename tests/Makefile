include ../config.mk

APPS  = mklargefile cachecheck parsecheck randomcheck
APPS_INST  = 

# ls *.c | sed "s%\.c%.o%g" | tr "\n" " " >> Makefile

mklargefile_cflags = ${CFLAGS}
mklargefile_ldflags = 
mklargefile_objs = mklargefile.c
cachecheck_cflags = ${CFLAGS}
cachecheck_ldflags = ${LDFLAGS} ${LIBS}
cachecheck_objs = ../server/fifocache.o cachecheck.o
parsecheck_cflags = ${CFLAGS}
parsecheck_ldflags = ${LDFLAGS} ${LIBS}
parsecheck_objs = parsecheck.o ../server/path.o
randomcheck_cflags = ${CFLAGS}
randomcheck_ldflags = ${LDFLAGS} ${LIBS}
randomcheck_objs = randomcheck.c

LDFLAGS += 
#CPPFLAGS += -I..
CFLAGS += -I..
LIBS   += 

all: $(APPS)

mklargefile: $(mklargefile_objs)
	$(CC) $(mklargefile_cflags) $(mklargefile_objs) -o $@ $(mklargefile_ldflags)

cachecheck: $(cachecheck_objs)
	$(CC) $(cachecheck_cflags) $(cachecheck_objs) -o $@ $(cachecheck_ldflags)

parsecheck: $(parsecheck_objs)
	$(CC) $(parsecheck_cflags) $(parsecheck_objs) -o $@ $(parsecheck_ldflags)

randomcheck: $(randomcheck_objs)
	$(CC) $(randomcheck_cflags) $(randomcheck_objs) -o $@ $(randomcheck_ldflags)

strip: $(APPS)
	$(STRIP) $(APPS)

distclean: clean

clean:
	rm -f *.o *~ $(APPS)

install: all 

install-strip: strip install

uninstall: 

