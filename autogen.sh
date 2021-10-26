#! /bin/sh -e
# This scripts rebuilds configure files using autoconf tools.
# Supports both FreeBSD and Linux installations. No copyrights
# script is public domain.
#
# I am not big fan of autotools stuff.
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
    if [ -x /usr/local/bin/autoconf259 ]; then
        echo "* Using autoconf 2.59"
    	AUTOCONF=autoconf259; export AUTOCONF
        AUTOHEADER=autoheader259; export AUTOHEADER
    else
	echo "* Using system default autoconf"
	AUTOCONF=autoconf
	AUTOHEADER=autoheader
    fi
    if [ -x /usr/local/bin/automake19 ]; then
	echo "* Using automake 1.9"
        ACLOCAL=aclocal19; export ACLOCAL
        AUTOMAKE=automake19; export AUTOMAKE
    elif [ -x /usr/local/bin/automake18 ]; then
	echo "* Using automake 1.8"
        ACLOCAL=aclocal18; export ACLOCAL
        AUTOMAKE=automake18; export AUTOMAKE
    else
	echo "* Using system default automake"
	ACLOCAL=aclocal
	AUTOMAKE=automake
    fi
    #Use autoconf 2.59 + automake 1.X pair
    export LDFLAGS=-L/usr/local/lib
    echo "Running $ACLOCAL"
    $ACLOCAL -I /usr/local/share/aclocal
    echo "Running $AUTOCONF"
    $AUTOCONF
    echo "Running $AUTOHEADER"
    $AUTOHEADER
    echo "Running $AUTOMAKE"
    $AUTOMAKE -a
    #autoreconf259 -iv -I /usr/local/share/aclocal
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
