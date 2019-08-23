#
# SCons fspcan build requested tester
#
# Version 1.0
# 23-Aug-2019
#

from SCons.Script import ARGUMENTS

def checkForBuildingFspscan(conf):
    """Check command line arguments if user requested to not build fspscan."""
    conf.Message("Checking if we are building fspscan... ")
    buildlamer=ARGUMENTS.get('without-fspscan', 0)
    try:
         buildlamer2=int(buildlamer)
    except ValueError:
         buildlamer2=None
    if buildlamer2 == 1 or str(buildlamer).lower() == 'yes':
        conf.Result(0)
        return False
    else:
        conf.Result(1)
        return True
