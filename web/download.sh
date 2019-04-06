#!/bin/sh

# ./download.sh NN http://kozos.jp/kozos/h8/h8_NN

if [ ! -e $1 ]; then
  mkdir $1
  mkdir -p $1/bootload
  mkdir -p $1/os
fi

bootload_files="defines.h dram.c dram.h elf.c elf.h interrupt.c interrupt.h intr.S intr.h lib.c lib.h main.c serial.c serial.h srec.c srec.h startup.s vector.c xmodem.c xmodem.h Makefile ld.scr"

os_files="command.c consdrv.c consdrv.h defines.h  ether.c ether.h etherdrv.c etherdrv.h interrupt.c interrupt.h intr.h ip.c kozos.c kozos.h lib.c lib.h main.c serial.c serial.h startup.s syscall.c syscall.h timer.c timer.h timerdrv.c timerdrv.h Makefile ld.scr"

( cd $1/bootload;
  for f in $bootload_files; do
    wget $2/bootload/$f -O $f.euc
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
