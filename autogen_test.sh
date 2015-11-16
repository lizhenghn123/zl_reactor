#!/bin/bash


case `uname` in
    Darwin) libtoolize=glibtoolize ;;
    *)      libtoolize=libtoolize  ;;
esac

INSTALL_DIR=$(cd "$(dirname "$0")"; pwd)

#make distclean
set -x
$libtoolize --force --automake --copy
aclocal -I build/acmacros
automake --foreign --add-missing --copy
autoconf
CFLAGS=-m64 CXXFLAGS=-m64 LDFLAGS=-m64 ./configure --prefix=$INSTALL_DIR  enable_test_suites=yes
make 
make install
