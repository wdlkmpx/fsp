include ../config.mk

LIB_STATIC = libcommon.a

# ls *.c | sed "s%\.c%.o%g" | tr "\n" " " >> Makefile
OBJS =  udp_io.o getsl.o

#CPPFLAGS += -I..
CFLAGS += -I..
LIBS   += 

all: $(LIB_STATIC)  

$(LIB_STATIC): $(OBJS)
	$(AR) rcs $(LIB_STATIC) $(OBJS)

strip:

distclean: clean

clean:
	rm -f *.o *~ *.a *.so 

install: $(LIB_STATIC)

install-strip: install

uninstall:



