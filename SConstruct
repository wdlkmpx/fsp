# Process this file with http://www.scons.org to build FSP

# init Scons
EnsureSConsVersion(0,96)
PREFIX='/usr/local'
PACKAGE='fsp'
VERSION='2.8.1b24'

env = Environment(CPPPATH='#/include')
# Turn CPPFLAGS to list
env.Append( CPPFLAGS = [])

#################### Tests ###################

# check for other GCC options
def checkForGCCOption(conf,option):
   if not conf.env['CC'].startswith('gcc'):
       return 0
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

############  Start configuration ##############
conf = Configure(env,{'checkForGCCOption':checkForGCCOption,
                      'MAINTAINER_MODE':checkForMaintainerMode,
		      'checkForLockPrefix':checkForLockPrefix,
		      'checkPrefix':checkForUserPrefix
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
conf.MAINTAINER_MODE()
conf.Finish()

#configure globals
TARBALL=PACKAGE+'-'+VERSION+'.tar.gz'
env.Append(CPPFLAGS = "-DPACKAGE_VERSION=\\\""+VERSION+"\\\"")
# process build rules
Export( Split("env PREFIX PACKAGE VERSION TARBALL"))
env.SConscript(dirs=Split('. bsd_src common server client clients contrib'))