# ステップ5

## ELF形式の展開

```bash
$ vi elf.h, elf.c
$ vi Makefile, main.c
$ make
$ make write
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx kzload-4.elf
Sending kzload-4.elf, 38 blocks: Give your local XMODEM receive command now.
Bytes Sent:   4992   BPS:428

Transfer complete

XMODEM receive succeeded.
kzload> dump
size: 1380
7f 45 4c 46 01 02 01 00  00 00 00 00 00 00 00 00
00 02 00 2e 00 00 00 01  00 00 01 00 00 00 00 34
00 00 11 74 00 81 00 00  00 34 00 20 00 04 00 28
...

kzload> run
0000b4 00000000 00000000 00100 00100 06 01
0001b4 00000100 00000100 0057e 0057e 05 01
000734 00000680 00000680 000a4 000a4 04 01
0007d8 00fffc20 00000724 00004 00018 06 01
kzload> ~
[EOT]
$ h8300-elf-readelf -l kzload-4.elf
Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x0000b4 0x00000000 0x00000000 0x00100 0x00100 RW  0x1
  LOAD           0x0001b4 0x00000100 0x00000100 0x0057e 0x0057e R E 0x1
  LOAD           0x000734 0x00000680 0x00000680 0x000a4 0x000a4 R   0x1
  LOAD           0x0007d8 0x00fffc20 0x00000724 0x00004 0x00018 RW  0x1
```
