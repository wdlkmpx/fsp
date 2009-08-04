#
# SCons debug mode build tester
#
# Version 1.0
# 04-Aug-2009
#

from SCons.Script import ARGUMENTS

def checkForDebugBuild(conf):
    """Check command line arguments if user requested debug mode build."""
    conf.Message("checking if we are building with extra debug code... ")
    buildlamer=ARGUMENTS.get('enable-debug', 0)
    if buildlamer == 0 or str(buildlamer).lower() == 'no':
        conf.Result(0)
    else:
        conf.env.Append(CPPFLAGS = '-DDEBUG')
        conf.Result(1)
