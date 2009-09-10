#
# SCons SGML Doc building check
#
# Version 1.0
# 10-Sep-2009
#

from SCons.Script import ARGUMENTS,Touch,Delete
import os
import SCons.Action

def checkForSGMLFMT(check):
    """Returns True if we have working sgmlfmt executable for building documentation."""
    check.Message("Checking for working sgmlfmt... ")
    oldp=SCons.Action.print_actions
    SCons.Action.print_actions = 0
    check.env.Execute(Touch('empty'))
    check.env.Execute(Delete('empty.html'))
    rc,out=check.TryAction("sgmlfmt -d docbook -f html empty")
    if os.path.isfile('empty.html'):
        rc = 0
    else:
        rc = 127
    check.env.Execute(Delete(['empty','empty.html']))
    SCons.Action.print_actions = oldp
    if rc == 0:
        check.Result(True)
        return True
    else:
        check.Result(False)
        return False
