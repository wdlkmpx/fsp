#! /bin/sh -e
# This scripts rebuilds configure files using autoconf tools.
# Supports both FreeBSD and Linux installations. No copyrights
# script is public domain.
#
# I am not big fan of autotools stuff, but other solutions
# like scons are worse (harder to maintain).
#
#                           Radim Kolar
#
rm -f configure configure.lineno config.log config.status
rm -f aclocal.m4
#rm -fr autom4te.cache
rm -f Makefile "Makefile.in"
echo "Generating configure and friends..."
if [ `uname -s` = 'FreeBSD' ]; then
    echo "* FreeBSD detected"
    echo "* Using autoconf 2.59"
    if [ -x /usr/local/bin/automake18 ]; then
	echo "* Using automake 1.8"
        ACLOCAL=aclocal18; export ACLOCAL
        AUTOMAKE=automake18; export AUTOMAKE
    else
	echo "* Using automake 1.9"
        ACLOCAL=aclocal19; export ACLOCAL
        AUTOMAKE=automake19; export AUTOMAKE
    fi	
    #Use autoconf 2.59 + automake 1.X pair
    AUTOHEADER=autoheader259; export AUTOHEADER
    AUTOCONF=autoconf259; export AUTOCONF
    export LDFLAGS=-L/usr/local/lib
    autoreconf259 -iv
else
    echo "Using your default auto* tools"
    #this should work with recent autotools
    autoreconf -iv
fi

if [ $# -eq 0 ]; then
  echo "Now running configure in maintainer mode"
  ./configure --enable-maintainer-mode
else
  echo "Now running configure $@"
  ./configure $@
fi;
echo "$0 done."
