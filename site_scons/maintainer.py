#
# SCons check for maintainer mode
#
# Version 1.1
# 24-Jul-2009
#

from SCons.Script import ARGUMENTS

def checkForMaintainerMode(conf):
    """Check if user wants to enable maintainer compilation mode."""
    conf.Message("checking whether to enable maintainer mode... ")
    if ARGUMENTS.get('maintainer-mode', 0) or \
       ARGUMENTS.get('enable-maintainer-mode', 0):
			  conf.Result(1)
			  conf.env.Append(CCFLAGS = '-O0')
			  conf.env.Append(CPPFLAGS = '-DMAINTAINER_MODE')
                          return True
    else:
			  conf.Result(0)
                          return False
