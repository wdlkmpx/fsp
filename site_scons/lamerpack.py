#
# SCons lamerpack building tester
#
# Version 1.0
# 02-Aug-2009
#

from SCons.Script import ARGUMENTS

def checkForLamerpack(conf):
    """Check command line arguments if user requested lamerpack build."""
    conf.Message("checking if we are building lamer pack... ")
    buildlamer=ARGUMENTS.get('enable-lamerpack', 0)
    if buildlamer == 0 or str(buildlamer).lower() == 'no':
        conf.Result(0)
    else:
        conf.env.Append(CPPFLAGS = '-DLAMERPACK')
        conf.Result(1)
