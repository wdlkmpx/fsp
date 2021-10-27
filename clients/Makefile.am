## Process this file with automake to produce Makefile.in

bin_PROGRAMS= \
	fcatcmd \
	fcdcmd \
	fducmd \
	ffindcmd \
	fgetcmd \
	fgrabcmd \
	fhostcmd \
	flscmd \
	fmkdir \
	fprocmd \
	fput \
	frmcmd \
	frmdircmd \
	fver \
	fspmerge \
	fstatcmd \
	fmvcmd \
	fbye \
	fsetupcmd

LDADD=-L../common -L../bsd_src -L../client -lclient -lcommon -lbsdfsp $(EX_LIBS)

fspmerge_SOURCES=merge.c printpro.c fspprof.l
fcdcmd_SOURCES=fcdcmd.c printpro.c
fprocmd_SOURCES=printpro.c fprocmd.c
noinst_HEADERS=printpro.h fhost.h merge.h
fhostcmd_SOURCES=fhostcmd.c fspprof.l fhost.c
fhostcmd_LDADD=$(LDADD)
fsetupcmd_SOURCES=fsetupcmd.c fspsites.l fhost.c
fsetupcmd_LDADD=$(LDADD)

fspmerge_LDADD= \
	fcatcmd_m.o \
	fcdcmd_m.o \
	fducmd_m.o \
	ffindcmd_m.o \
	fgetcmd_m.o \
    fgrabcmd_m.o \
    fhostcmd_m.o \
    flscmd_m.o \
    fmkdir_m.o \
    fprocmd_m.o \
    fput_m.o \
    frmcmd_m.o \
    frmdircmd_m.o \
    fver_m.o \
    fmvcmd_m.o \
    fstatcmd_m.o \
    fbye_m.o \
    fhost_m.o \
    fsetupcmd_m.o \
    $(LDADD)

AM_CFLAGS:=-DSYSCONFDIR="\"@sysconfdir@\"" $(AM_CFLAGS)


fcatcmd_m.o:
	@echo '#include "fcatcmd.c"' > fcatcmd_m.c
	$(COMPILE) -Dmain=fcatcmd_main -c fcatcmd_m.c
	@rm -f fcatcmd_m.c
fcdcmd_m.o: 
	@echo '#include "fcdcmd.c"' > fcdcmd_m.c
	${COMPILE} -Dmain=fcdcmd_main -c fcdcmd_m.c
	@rm -f fcdcmd_m.c
fducmd_m.o:
	@echo '#include "fducmd.c"' > fducmd_m.c
	${COMPILE} -Dmain=fducmd_main -c fducmd_m.c
	@rm -f fducmd_m.c
ffindcmd_m.o:
	@echo '#include "ffindcmd.c"' > ffindcmd_m.c
	${COMPILE} -Dmain=ffindcmd_main -c ffindcmd_m.c
	@rm -f ffindcmd_m.c
fgetcmd_m.o:
	@echo '#define COMMAND_GET' > fgetcmd_m.c
	@echo '#include "fgetcmd.c"' >> fgetcmd_m.c
	${COMPILE} -Dmain=fgetcmd_main -c fgetcmd_m.c
	@rm -f fgetcmd_m.c
fgrabcmd_m.o:
	@echo '#include "fgrabcmd.c"' > fgrabcmd_m.c
	${COMPILE} -Dmain=fgrabcmd_main -c fgrabcmd_m.c
	@rm -f fgrabcmd_m.c
fhostcmd_m.o:
	@echo '#include "fhostcmd.c"' > fhostcmd_m.c
	${COMPILE} -Dmain=fhostcmd_main -c fhostcmd_m.c
	@rm -f fhostcmd_m.c
flscmd_m.o:
	@echo '#include "flscmd.c"' > flscmd_m.c
	${COMPILE} -Dmain=flscmd_main -c flscmd_m.c
	@rm -f flscmd_m.c
fmkdir_m.o:
	@echo '#include "fmkdir.c"' > fmkdir_m.c
	${COMPILE} -Dmain=fmkdir_main -c fmkdir_m.c
	@rm -f fmkdir_m.c
fprocmd_m.o:
	@echo '#include "fprocmd.c"' > fprocmd_m.c
	$(COMPILE) -Dmain=fprocmd_main -c fprocmd_m.c
	@rm -f fprocmd_m.c
fput_m.o:
	@echo '#include "fput.c"' > fput_m.c
	${COMPILE} -Dmain=fput_main -c fput_m.c
	@rm -f fput_m.c
frmcmd_m.o:
	@echo '#include "frmcmd.c"' > frmcmd_m.c
	${COMPILE} -Dmain=frmcmd_main -c frmcmd_m.c
	@rm -f frmcmd_m.c
frmdircmd_m.o:
	@echo '#include "frmdircmd.c"' > frmdircmd_m.c
	${COMPILE} -Dmain=frmdircmd_main -c frmdircmd_m.c
	@rm -f frmdircmd_m.c
fver_m.o:
	@echo '#include "fver.c"' > fver_m.c
	${COMPILE} -Dmain=fver_main -c fver_m.c
	@rm -f fver_m.c
fstatcmd_m.o:
	@echo '#include "fstatcmd.c"' > fstatcmd_m.c
	${COMPILE} -Dmain=fstatcmd_main -c fstatcmd_m.c
	@rm -f fstatcmd_m.c
fmvcmd_m.o:
	@echo '#include "fmvcmd.c"' > fmvcmd_m.c
	${COMPILE} -Dmain=fmvcmd_main -c fmvcmd_m.c
	@rm -f fmvcmd_m.c
fbye_m.o:
	@echo '#include "fbye.c"' > fbye_m.c
	${COMPILE} -Dmain=fbye_main -c fbye_m.c
	@rm -f fbye_m.c
fhost_m.o:
	@echo '#include "fhost.c"' > fhost_m.c
	${COMPILE} -Dmain=fhost_main -c fhost_m.c
	@rm -f fhost_m.c
fsetupcmd_m.o:
	@echo '#include "fsetupcmd.c"' > fsetupcmd_m.c
	${COMPILE} -Dmain=fsetupcmd_main -c fsetupcmd_m.c fspsites.l fhost.c
	@rm -f fsetupcmd_m.c
