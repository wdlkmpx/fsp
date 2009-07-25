# Process this file with http://www.scons.org to build FSP

import os
# init Scons
EnsureSConsVersion(0,96)
EnsurePythonVersion(2,2)

# set defaults
PREFIX='/usr/local'
PACKAGE='fsp'
VERSION='2.8.1b25'
EFENCE=False

env = Environment(CPPPATH='#/include', LIBPATH=['/usr/lib','/usr/local/lib'])

#import environment
from importer import importEnvironment,importVariable
importEnvironment(env,'HOME')
importVariable(env,'CC')
importVariable(env,'CFLAGS','CCFLAGS')
importEnvironment(env,prefix='DISTCC_')
importEnvironment(env,prefix='CCACHE_')

# Turn CPPFLAGS to list, so we can add values to it
env.Append( CPPFLAGS = [])

# Get CC from commandline
if ARGUMENTS.get('CC', 0):
    env.Replace(CC =  ARGUMENTS.get('CC'))

if ARGUMENTS.get('CFLAGS',0):
   env.Replace(CCFLAGS = ARGUMENTS.get('CFLAGS'))
if ARGUMENTS.get('CCFLAGS',0):
   env.Replace(CCFLAGS = ARGUMENTS.get('CCFLAGS'))

# Convert CCFLAGS into list
env.Replace(CCFLAGS = str(env['CCFLAGS']).split(' '))

############  Start configuration   ##############

from maintainer import checkForMaintainerMode
from compilertest import checkForCCOption
from prefix import checkForUserPrefix
from lockprefix import checkForLockPrefix
from clangtest import getVariableSize

conf = Configure(env,{'checkForCCOption':checkForCCOption,
                      'MAINTAINER_MODE':checkForMaintainerMode,
		      'checkForLockPrefix':checkForLockPrefix,
		      'checkPrefix':checkForUserPrefix,
		      'sizeOf':getVariableSize
		      })
# check for CC options
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
       conf.checkForCCOption(option)

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
PREFIX=conf.checkPrefix(PREFIX)
conf.env.Append(CPPFLAGS = '-DSYSCONFDIR=\\"'+PREFIX+'/etc\\"')
EFENCE = conf.MAINTAINER_MODE()
if EFENCE == True:
    EFENCE=conf.CheckLib("efence","EF_Abort")
conf.Finish()

#configure globals
TARBALL=PACKAGE+'-'+VERSION+'.tar.gz'
env.Append(CPPFLAGS = "-DPACKAGE_VERSION=\\\""+VERSION+"\\\"")
# process build rules
Export( Split("env PREFIX PACKAGE VERSION TARBALL"))
env.SConscript(dirs=Split('. bsd_src common server client clients contrib tests'))
