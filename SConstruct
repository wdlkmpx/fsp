# Process this file with http://www.scons.org to build FSP

import os
# init Scons
EnsureSConsVersion(0,96)
PREFIX='/usr/local'
PACKAGE='fsp'
VERSION='2.8.1b24'

env = Environment(CPPPATH='#/include')
# Turn CPPFLAGS to list
env.Append( CPPFLAGS = [])

################### Functions ######################
def importEnv(list=None, prefix=None):
   if list:
       for i in list:
	   if os.environ.get(i):
	       kw={}
	       kw[i]=os.environ.get(i)
	       kw={ 'ENV': kw }
	       env.Append(**kw)
   if prefix:
       for i in os.environ.keys():
	   if i.startswith(prefix):
	       kw={}
	       kw[i]=os.environ.get(i)
	       kw={ 'ENV': kw }
	       env.Append(**kw)

#import environment
importEnv(['HOME','CC'])
importEnv(prefix='DISTCC_')
importEnv(prefix='CCACHE_')
if env['ENV'].get('CC'):
    env.Replace( CC = env['ENV'].get('CC'))
# Get CC from commandline
if ARGUMENTS.get('CC', 0):
    env.Replace(CC =  ARGUMENTS.get('CC'))

#################### Tests ###################

# check for other GCC options
def checkForGCCOption(conf,option):
   conf.Message("checking whether GCC supports "+option+" ")
   lastCFLAGS=conf.env['CCFLAGS']
   conf.env.Append(CCFLAGS = option)
   rc = conf.TryCompile("""
void dummy(void);
void dummy(void) {}
""",'.c')
   if not rc:
       conf.env.Replace(CCFLAGS = lastCFLAGS)
   conf.Result(rc)
   return rc

# check for maintainer mode
def checkForMaintainerMode(conf):
    conf.Message("checking whether to enable maintainer mode... ")
    if ARGUMENTS.get('maintainer-mode', 0) or \
       ARGUMENTS.get('enable-maintainer-mode', 0):
			  conf.Result(1)
			  conf.env.Append(CCFLAGS = '-O0')
			  conf.env.Append(CPPFLAGS = '-DMAINTAINER_MODE')
    else:
			  conf.env.Append(CCFLAGS = '-O')
			  conf.Result(0)

# check for user-supplied lock prefix
def checkForLockPrefix(conf):
    conf.Message("checking for user supplied lockprefix... ")
    lp = ARGUMENTS.get('lockprefix', 0) or ARGUMENTS.get("with-lockprefix",0)

    if lp:
			  conf.Result(1)
			  conf.env.Append(CPPFLAGS = '-DFSP_KEY_PREFIX=\\"'+lp+'\\"')
    else:
			  conf.Result(0)

# check for user-supplied prefix
def checkForUserPrefix(conf):
    global PREFIX
    conf.Message("checking for user supplied prefix... ")
    lp = ARGUMENTS.get('prefix', 0)
    if lp:
			  conf.Result(1)
			  PREFIX=lp
    else:
			  conf.Result(0)

def getVarSize(conf,var):
    conf.Message("checking for size of "+var+" ")
    rc = conf.TryCompile("""
#include <stdio.h>
#include <sys/types.h>    
      
main ()
{
 if ((%s *) 0)
   return 0;
 if (sizeof (%s))
   return 0;
 ;
 return 0;
}
""" % (var,var),'.c')
    if rc:
	rc,result = conf.TryRun('''
#include <stdio.h>
#include <sys/types.h>	

main ()
{
    printf("%%d",sizeof(%s));
    return 0;
}''' % var,'.c')
	if rc:
	    rc=result
    conf.Result(rc)
    return rc

############  Start configuration ##############
conf = Configure(env,{'checkForGCCOption':checkForGCCOption,
                      'MAINTAINER_MODE':checkForMaintainerMode,
		      'checkForLockPrefix':checkForLockPrefix,
		      'checkPrefix':checkForUserPrefix,
		      'sizeOf':getVarSize
		      })
#check for GCC options
for option in Split("""
      -Wall -W -Wstrict-prototypes -Wmissing-prototypes -Wshadow
      -Wbad-function-cast -Wcast-qual -Wcast-align -Wwrite-strings
      -Waggregate-return -Wmissing-declarations
      -Wmissing-format-attribute -Wnested-externs
      -ggdb -fno-common -Wchar-subscripts -Wcomment
      -Wimplicit -Wsequence-point -Wreturn-type
      -Wfloat-equal -Wno-system-headers -Wredundant-decls
      -Wmissing-noreturn -pedantic
      -Wlong-long -Wundef -Winline
      -Wpointer-arith -Wno-unused-parameter
      -Wunreachable-code
"""):
       conf.checkForGCCOption(option)
#portability build time config
if conf.CheckFunc('srandomdev'):
    conf.env.Append(CPPFLAGS = '-DHAVE_SRANDOMDEV')
if conf.CheckFunc('fseeko'):
    conf.env.Append(CPPFLAGS = '-DHAVE_FSEEKO')
if conf.CheckFunc('random'):
    conf.env.Append(CPPFLAGS = '-DHAVE_RANDOM')
if conf.CheckFunc('fork'):
    conf.env.Append(CPPFLAGS = '-DHAVE_FORK')
if conf.CheckFunc('setsid'):
    conf.env.Append(CPPFLAGS = '-DHAVE_SETSID')
if conf.CheckCHeader('unistd.h'):
    env.Append(CPPFLAGS = '-DHAVE_UNISTD_H')
env.Append(CPPFLAGS = '-DSIZEOF_CHAR='+conf.sizeOf("char"))
env.Append(CPPFLAGS = '-DSIZEOF_LONG='+conf.sizeOf("long"))
env.Append(CPPFLAGS = '-DSIZEOF_SHORT='+conf.sizeOf("short"))
env.Append(CPPFLAGS = '-DSIZEOF_UNSIGNED='+conf.sizeOf("unsigned"))
env.Append(CPPFLAGS = '-DSIZEOF_VOID='+conf.sizeOf("void"))
env.Append(CPPFLAGS = '-DSIZEOF_OFF_T='+conf.sizeOf("off_t"))

#check for available locking methods
if not conf.CheckType("union semun", "#include <sys/types.h>\n#include <sys/ipc.h>\n#include <sys/sem.h>",'c'):
       conf.env.Append(CPPFLAGS = "-D_SEM_SEMUN_UNDEFINED=1")
fun_lockf=conf.CheckFunc("lockf")
fun_semop=conf.CheckFunc("semop")
fun_shmget=conf.CheckFunc("shmget")
fun_flock=conf.CheckFunc("flock")

#select locking type
lt=ARGUMENTS.get('locking', 0) or ARGUMENTS.get("with-locking",0) or ARGUMENTS.get("lock",0) or ARGUMENTS.get("with-lock",0)
if lt == "none":
    conf.env.Append(CPPFLAGS = '-DFSP_NOLOCKING')
elif lt == "lockf" and fun_lockf:
    conf.env.Append(CPPFLAGS = '-DFSP_USE_LOCKF')
elif lt == "semop" and fun_semop and fun_shmget:
    conf.env.Append(CPPFLAGS = '-DFSP_USE_SHAREMEM_AND_SEMOP')
elif lt == "shmget" and fun_shmget and fun_lockf:
    conf.env.Append(CPPFLAGS = '-DFSP_USE_SHAREMEM_AND_LOCKF')
elif lt == "flock" and fun_flock:
    conf.env.Append(CPPFLAGS = '-DFSP_USE_FLOCK')
#AUTODETECT best available locking type    
elif fun_semop and fun_shmget:
    conf.env.Append(CPPFLAGS = '-DFSP_USE_SHAREMEM_AND_SEMOP')
elif fun_shmget and fun_lockf:
    conf.env.Append(CPPFLAGS = '-DFSP_USE_SHAREMEM_AND_LOCKF')
elif fun_lockf:
    conf.env.Append(CPPFLAGS = '-DFSP_USE_LOCKF')
elif fun_flock:
    conf.env.Append(CPPFLAGS = '-DFSP_USE_FLOCK')
else:
    conf.env.Append(CPPFLAGS = '-DFSP_NOLOCKING')
conf.checkForLockPrefix()
conf.checkPrefix()
conf.env.Append(CPPFLAGS = '-DSYSCONFDIR=\\"'+PREFIX+'/etc\\"')
conf.MAINTAINER_MODE()
conf.Finish()

#configure globals
TARBALL=PACKAGE+'-'+VERSION+'.tar.gz'
env.Append(CPPFLAGS = "-DPACKAGE_VERSION=\\\""+VERSION+"\\\"")
# process build rules
Export( Split("env PREFIX PACKAGE VERSION TARBALL"))
env.SConscript(dirs=Split('. bsd_src common server client clients contrib tests'))
