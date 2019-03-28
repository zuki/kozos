#!/bin/sh

# ./download.sh NN http://kozos.jp/kozos/h8/h8_NN

if [ ! -e $1 ]; then
  mkdir $1
  mkdir -p $1/kzload
  mkdir -p $1/os
fi

( cd $1/kzload;
  for f in elf.c elf.h lib.c lib.h main.c serial.c serial.h srec.c srec.h startup.s xmodem.c xmodem.h ld.scr make.sh; do
    wget $2/kzload/$f -O $f.euc
  	nkf -w $f.euc > $f
    rm $f.euc
  done
)

( cd $1/os;
  for f in lib.c lib.h main.c serial.c serial.h startup.s ld.scr make.sh; do
    wget $2/os/$f -O $f.euc
  	nkf -w $f.euc > $f
    rm $f.euc
  done
)
