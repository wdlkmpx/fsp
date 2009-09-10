#
# SCons SGML Doc building check
#
# Version 1.0
# 10-Sep-2009
#

from SCons.Script import ARGUMENTS,Touch,Delete
import os

def checkForSGMLFMT(check):
    """Returns True if we have working sgmlfmt executable for building documentation."""
    check.Message("Checking for working sgmlfmt... ")
    Touch('empty')
    Delete('empty.html')
    rc,out=check.TryAction("sgmlfmt -d docbook -f html empty")
    if os.path.isfile('empty.html'):
        rc = 0
    else:
        rc = 127
    Delete(['empty','empty.html'])
    if rc == 0:
        check.Result(True)
        return True
    else:
        check.Result(False)
        return False
