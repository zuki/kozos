#!/bin/sh

# ./download.sh NN http://kozos.jp/kozos/h8/h8_NN

if [ ! -e $1 ]; then
  mkdir $1
  mkdir -p $1/kzload
  mkdir -p $1/os
fi

kzload_files="elf.c elf.h interrupt.c interrupt.h intr.S lib.c lib.h main.c serial.c serial.h srec.c srec.h startup.s types.h uudecode.c uudecode.h vector.c xmodem.c xmodem.h Makefile ld.scr make.sh"

os_files="command.c configure.h extintr.c extintr.h idle.c interrupt.c interrupt.h kozos.c kozos.h memory.c memory.h lib.c lib.h main.c serial.c serial.h startup.s syscall.c syscall.h thread.c thread.h timer.c timer.h types.h ld.scr make.sh Makefile"

( cd $1/kzload;
  for f in $kzload_files; do
    wget $2/kzload/$f -O $f.euc
  	nkf -w $f.euc > $f
    rm $f.euc
  done
)

( cd $1/os;
  for f in $os_files; do
    wget $2/os/$f -O $f.euc
  	nkf -w $f.euc > $f
    rm $f.euc
  done
)
