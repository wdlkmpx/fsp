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
# TODO: add linux support (sven?)
#       add detection of automake19 and use it instead of
#           automake18
#
rm -f configure configure.lineno config.log config.status
rm -f aclocal.m4
#rm -fr autom4te.cache
rm -f Makefile "Makefile.in"
echo "Generating configure and friends..."
if [ `uname -s` = 'FreeBSD' ]; then
    echo "* FreeBSD detected"
    echo "* Using autoconf 2.59 + automake 1.8"
    #Use autoconf 2.59 + automake 1.8 pair
    ACLOCAL=aclocal18; export ACLOCAL
    AUTOMAKE=automake18; export AUTOMAKE
    AUTOHEADER=autoheader259; export AUTOHEADER
    AUTOCONF=autoconf259; export AUTOCONF
    autoreconf259 -v
else
    echo "Using your default auto* tools"
    #this should work with recent autotools
    autoreconf -iv
fi

echo "Now running configure $@"
./configure $@
echo "done."
