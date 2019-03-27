# ステップ6

## kzloaderの修正

```bash
$ vi elf.c
$ vi elf.h
$ vi main.c
$ make
$ make write
```

## kozosの作成

```bash
$ mkdir os
$ cd os
$ cp ../bootloader/defines.h .
$ cp ../bootload/lib.[hc] .
$ cp ../bootload/serial.[hc] .
$ cp ../bootload/main.c .
$ cp ../bootload/ld.scr .
$ cp ../bootload/Makefile .
$ vi main.c
$ vi ld.scr
$ vi Makefile
$ make
/usr/local/h8/h8-elf/bin/h8300-elf-gcc -c -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -Wno-pointer-sign -DKOZOS startup.s
/usr/local/h8/h8-elf/bin/h8300-elf-gcc -c -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -Wno-pointer-sign -DKOZOS main.c
/usr/local/h8/h8-elf/bin/h8300-elf-gcc -c -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -Wno-pointer-sign -DKOZOS lib.c
/usr/local/h8/h8-elf/bin/h8300-elf-gcc -c -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -Wno-pointer-sign -DKOZOS serial.c
/usr/local/h8/h8-elf/bin/h8300-elf-gcc startup.o main.o lib.o serial.o -o kozos -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -Wno-pointer-sign -DKOZOS -static -T ld.scr -L.
cp kozos kozos.elf
/usr/local/h8/h8-elf/bin/h8300-elf-strip kozos
$ ls -l
-rwxr-xr-x  1 dspace  staff  1300  3 27 11:05 kozos
-rwxr-xr-x  1 dspace  staff  3160  3 27 11:05 kozos.elf
-rwxr-xr-x  1 dspace  staff  1300  3 27 11:05 kozos
-rwxr-xr-x  1 dspace  staff  3160  3 27 11:05 kozos.elf
$ h8300-elf-readelf -a kozos.elf
ELF Header:
  Entry point address:               0xffc020

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 1] .text             PROGBITS        00ffc020 000074 000334 00  AX  0   0  2
  [ 2] .rodata           PROGBITS        00ffc354 0003a8 000044 00   A  0   0  4
  [ 3] .bss              NOBITS          00ffc398 0003ec 000020 00  WA  0   0  1

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000000 0x00ffbfac 0x00ffbfac 0x003ec 0x003ec R E 0x1
  LOAD           0x0003ec 0x00ffc398 0x00ffc398 0x00000 0x00020 RW  0x1

$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx kozos
Sending kozos, 10 blocks: Give your local XMODEM receive command now.
Bytes Sent:   1408   BPS:318

Transfer complete

XMODEM receive succeeded.
kzload> run
starting from entry point: ffc020
Hello World!
> echo aaa
 aaa
> ei
unknown.
> exit
exit
~
[EOT]
$
```
