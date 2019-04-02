#!/bin/sh -x

# ディレクトリの指定
LOCAL=/usr/local
TARGET=h8300-elf
TARGETDIR=$LOCAL/$TARGET

PATH=$TARGETDIR/bin:$PATH:$BINDIR export PATH

# gcc がコンパイル時に as や ld を探すディレクトリを指定
# (gcc の -B オプションと等価)
GCC_EXEC_PREFIX=$TARGETDIR/bin export GCC_EXEC_PREFIX

AR=$TARGETDIR/bin/ar export AR
AS=$TARGETDIR/bin/as export AS
CC="$TARGETDIR/bin/gcc" export CC
CPP=`$TARGETDIR/bin/gcc -print-prog-name=cpp` export CPP
CXX=$CC export CXX
FC=$TARGETDIR/bin/f77 export FC
LD=$TARGETDIR/bin/ld export LD
NM=$TARGETDIR/bin/nm export NM
RANLIB=$TARGETDIR/bin/ranlib export RANLIB
SIZE=$TARGETDIR/bin/size export SIZE
ADDR2LINE=$TARGETDIR/bin/addr2line export ADDR2LINE
GASP=$TARGETDIR/bin/gasp export GASP
OBJCOPY=$TARGETDIR/bin/objcopy export OBJCOPY
OBJDUMP=$TARGETDIR/bin/objdump export OBJDUMP
STRINGS=$TARGETDIR/bin/strings export STRINGS
STRIP=$TARGETDIR/bin/strip export STRIP

HOSTED_CC=/usr/bin/cc export HOSTED_CC
NOGCCERROR=yes export NOGCCERROR
OBJECT_FMT=ELF export OBJECT_FMT

MAKE="gmake -f Makefile"; export MAKE

set -x
exec $MAKE "$@"
