#
# SCons DSSSL processor check
#
# Version 1.0
# 07-Sep-2014
#

import subprocess

def checkDSSSLProcessor(check, name="jade"):
    """Check if DSSSL engine is working. Returns True or False."""
    check.Message("Checking if DSSSL processor "+name+" works... ")
    try:
       echo = subprocess.Popen(('/bin/echo','""'), stdout = subprocess.PIPE )
       version = subprocess.check_output((name, '-v'), stdin = echo.stdout, stderr = subprocess.STDOUT )
    except subprocess.CalledProcessError as e:
       if "version" in e.output:
          check.Result(True)
          return True
    check.Result(False)
    return False
