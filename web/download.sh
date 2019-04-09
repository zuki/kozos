#!/bin/bash

# ./download.sh NN http://kozos.jp/kozos/h8/h8_NN

if [ ! -e $1 ]; then
  mkdir $1
  mkdir -p $1/bootload
  mkdir -p $1/os
fi

bootload_files="defines.h dram.c dram.h elf.c elf.h firmdata.scr interrupt.c interrupt.h intr.S intr.h lib.c lib.h main.c serial.c serial.h srec.c srec.h startup.s vector.c xmodem.c xmodem.h Makefile ld.scr"

os_files="arp.c arp.h clock.c command.c consdrv.c consdrv.h defines.h  ethernet.c ethernet.h httpd.c icmp.c icmp.h interrupt.c interrupt.h intr.s intr.h ip.c ip.h kozos.c kozos.h ld.scr lib.c lib.h main.c memory.c memory.h netdrv.c netdrv.h rtl8019.c rtl8019.h serial.c serial.h startup.s syscall.c syscall.h tcp.c tcp.h timer.c timer.h timerdrv.c timerdrv.h vector.c Makefile"

<<CONTENT
( cd $1/bootload;
  for f in $bootload_files; do
    wget $2/bootload/$f -O $f.euc
  	nkf -w $f.euc > $f
    rm $f.euc
  done
)
CONTENT

( cd $1/os;
  for f in $os_files; do
    wget $2/os/$f -O $f.euc
  	nkf -w $f.euc > $f
    rm $f.euc
  done
)
