# 移植編実装その5: [(H8移植編その２第３回)DRAM上で動かそう](http://kozos.jp/kozos/h8_2_03.html)

## 作業メモ

- DRAM関係のレジスタ構成は`6.1.4`。
- DRAMインタフェースの使用時の設定手順例は`6.5.12`の図6.34にあり、次のとおり。
    1. ABWCRの設定
    2. RTCORの設定
    3. RTMCSRのCKS2-0ビットの設定
    4. DRCRBの設定
    5. DRCRAの設定
    6. DRAM安定時間を確保


## bootloadの修正

```bash
$ vi dram.[hc]
$ vi main.c, ld.scr, Makefile
$ make clean && make
```

## osの修正

```bash
$ vi ld.scr
$ make clean && make && make image
```

## 実行

```bash
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx kozos.mot
Sending kozos.mot, 137 blocks: Give your local XMODEM receive command now.
Bytes Sent:  17664   BPS:1978

Transfer complete
                 XMODEM receive succeeded.
kzload> run
starting from entry point: 400100
kozos boot succeed!
command> timer
timer start.
command> expired.
command> ready.
ready.
ready.
timer
timer start.
command> ready.
expired.
command> ready.
ready.
kzload (kozos boot loader) started.
kzload> ~
[EOT]
```

## ファイルのアドレス確認

- **kzload.elf**

    os転送用のバッファは使用していないので、bzloadはDRAMを使用しない。

```bash
$ /usr/local/h8/h8-elf/bin/h8300-elf-readelf -a kzload.elf
[省略]

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .vectors          PROGBITS        00000000 0000b4 000100 00  WA  0   0  4
  [ 2] .text             PROGBITS        00000100 0001b4 000e16 00  AX  0   0  2
  [ 3] .rodata           PROGBITS        00000f18 000fcc 0001b4 00   A  0   0  4
  [ 4] .data             PROGBITS        00ffbf60 001180 000004 00  WA  0   0  4
  [ 5] .bss              NOBITS          00ffbf64 001184 000064 00  WA  0   0  4
  [ 6] .comment          PROGBITS        00000000 001184 000011 01  MS  0   0  1
  [ 7] .symtab           SYMTAB          00000000 001198 000ba0 10      8 137  4
  [ 8] .strtab           STRTAB          00000000 001d38 0003e2 00      0   0  1
  [ 9] .shstrtab         STRTAB          00000000 00211a 000046 00      0   0  1

[省略]
Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x0000b4 0x00000000 0x00000000 0x00100 0x00100 RW  0x1
  LOAD           0x0001b4 0x00000100 0x00000100 0x00e16 0x00e16 R E 0x1
  LOAD           0x000fcc 0x00000f18 0x00000f18 0x001b4 0x001b4 R   0x1
  LOAD           0x001180 0x00ffbf60 0x000010cc 0x00004 0x00068 RW  0x1
```

- **kozos.elf**

```bash
$ /usr/local/h8/h8-elf/bin/h8300-elf-readelf -a kozos.elf
[省略]
Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .text             PROGBITS        00400100 000074 0016b8 00  AX  0   0  2
  [ 2] .rodata           PROGBITS        004017b8 00172c 0000ec 00   A  0   0  4
  [ 3] .data             PROGBITS        004018a4 001818 000020 00  WA  0   0  4
  [ 4] .bss              NOBITS          004018c4 001838 00027c 00  WA  0   0  4
  [ 5] .comment          PROGBITS        00000000 001838 000011 01  MS  0   0  1
  [ 6] .symtab           SYMTAB          00000000 00184c 001080 10      7 188  4
  [ 7] .strtab           STRTAB          00000000 0028cc 0006ad 00      0   0  1
  [ 8] .shstrtab         STRTAB          00000000 002f79 00003d 00      0   0  1

[省略]
Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000000 0x0040008c 0x0040008c 0x01818 0x01818 R E 0x1
  LOAD           0x001818 0x004018a4 0x004018a4 0x00020 0x0029c RW  0x1
```
