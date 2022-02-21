include ../config.mk

APP  = fspd
# ls *.c | sed "s%\.c%.o%g" | tr "\n" " " >> Makefile
OBJS =  acl.o conf.o fifocache.o file.o host.o iprange.o log.o main.o path.o pidfile.o server.o

LDFLAGS +=  -L../common -lcommon $(EX_LIBS)
#CPPFLAGS += -I..
CFLAGS += -I..
LIBS   += 

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS)

strip: all
	$(STRIP) $(APP)

distclean: clean

clean:
	rm -f *.o *~ $(APP) $(APP).exe

install: all
	mkdir -p $(DESTDIR)$(bindir)
	install -c $(APP) $(DESTDIR)$(bindir)

install-strip: strip install

uninstall:
	rm -f $(DESTDIR)$(bindir)/$(APP)

