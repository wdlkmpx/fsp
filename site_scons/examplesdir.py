#
# SCons user-supplied docdir configuration
#
# Version 1.0
# 24-Aug-2019
#

from SCons.Script import ARGUMENTS
import os.path

def checkForUserExamplesdir(conf,olddir=None):
    """Returns examplesdir specified on command line or olddir if none is supplied."""
    conf.Message("Checking for user supplied examplesdir... ")
    lp = ARGUMENTS.get('examplesdir', 0)
    if lp:
       conf.Result(1)
       return lp
    else:
       conf.Result(0)
       return olddir
