# Sample sh initialization file.
#
# Tested in BASH and ZSH
###############################

# CODED BY Following wizards:
#  Humberto Massa - basic porting
#  Hanno Hecker   - resolved globing problem
#  Radim Kolar    - zsh setopt
#  Sven Hoexter   - minor fix
#
#  visit our homepage for porting story
#

export FSP_PORT=2000
export FSP_HOST=localhost
export FSP_DIR=/
export FSP_TRACE
export FSP_DELAY=3000

if [ -n "$ZSH_VERSION" ]; then 
 setopt sh_option_letters
fi

_fcat()   { fcatcmd "$@" ;set +f;}
alias fcat='set -f;_fcat'
_fstat()   { fstatcmd "$@" ;set +f;}
alias fstat='set -f;_fstat'
_fdu()    { fducmd "$@" ; set +f;}
alias fdu='set -f;_fdu'
_ffind()  { ffindcmd  "$@" ; set +f;}
alias ffind='set -f;_ffind'
_fget()   { fgetcmd   "$@" ; set +f;}
alias fget='set -f;_fget'
_fgrab()  { fgrabcmd  "$@" ; set +f;}
alias fgrab='set -f;_fgrab'
_fhost()  { eval $(fhostcmd "$@") ; set +f;}
alias fhost='set -f;_fhost'
_fless()  { fcatcmd   "$@" | less ; set +f;}
alias fless='set -f;_fless'
_fls()    { flscmd    "$@" ; set +f;}
alias fls='set -f;_fls'
_fmore()  { fcatcmd   "$@" | more ; set +f;}
alias fmore='set -f;_fmore'
_fpro()   { fprocmd   "$@"; set +f;}
alias fpro='set -f;_fpro'
fpwd()   { echo $FSP_DIR on $FSP_HOST port $FSP_PORT ;}
_frm()    { frmcmd    "$@"; set +f ;}
alias frm='set -f;_frm'
_frmdir() { frmdircmd "$@"; set +f ;}
alias frmdir='set -f;_frmdir'
ftouch() { touch "$1"; fput "$1"; rm "$1" ;}
_fcd()    { export FSP_DIR=$(fcdcmd "$@"); set +f;}
alias fcd='set -f;_fcd'
