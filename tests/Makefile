include ../config.mk

APPS  = mklargefile cachecheck parsecheck randomcheck
APPS_INST  = 

# ls *.c | sed "s%\.c%.o%g" | tr "\n" " " >> Makefile

mklargefile_cflags = 
mklargefile_ldflags = 
mklargefile_objs = mklargefile.c
cachecheck_cflags = 
cachecheck_ldflags = 
cachecheck_objs = ../server/fifocache.o cachecheck.o
parsecheck_cflags = 
parsecheck_ldflags = 
parsecheck_objs = parsecheck.o ../server/path.o
randomcheck_cflags = 
randomcheck_ldflags = 
randomcheck_objs = randomcheck.c

LDFLAGS += 
#CPPFLAGS += -I..
CFLAGS += -I..
LIBS   += 

all: $(APPS)

mklargefile: $(mklargefile_objs)
	$(CC) $(CFLAGS) $(mklargefile_cflags) $(mklargefile_objs) -o mklargefile $(LDFLAGS) $(LIBS) $(mklargefile_ldflags)

cachecheck: $(cachecheck_objs)
	$(CC) $(CFLAGS) $(cachecheck_cflags) $(cachecheck_objs) -o cachecheck $(LDFLAGS) $(LIBS) $(cachecheck_ldflags)

parsecheck: $(parsecheck_objs)
	$(CC) $(CFLAGS) $(parsecheck_cflags) $(parsecheck_objs) -o parsecheck $(LDFLAGS) $(LIBS) $(parsecheck_ldflags)

randomcheck: $(randomcheck_objs)
	$(CC) $(CFLAGS) $(randomcheck_cflags) $(randomcheck_objs) -o randomcheck $(LDFLAGS) $(LIBS) $(randomcheck_ldflags)

strip: $(APPS)
	$(STRIP) $(APPS)

distclean: clean

clean:
	rm -f *.o *~ $(APPS)

install: all 

install-strip: strip install

uninstall: 

