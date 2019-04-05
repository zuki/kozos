# ステップ3

## 3.1.1 静的変数の読み書きができない

```bash
$ make
/usr/local/h8/h8-elf/bin/h8300-elf-gcc -c -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -DKZLOAD main.c
/usr/local/h8/h8-elf/bin/h8300-elf-gcc vector.o startup.o main.o lib.o serial.o -o kzload -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -DKZLOAD -static -T ld.scr -L.
cp kzload kzload.elf
/usr/local/h8/h8-elf/bin/h8300-elf-strip kzload
$ make write
../tools/h8write/h8write -3069 -f20 kzload.mot /dev/cu.usbserial
H8/3069F is ready!  2002/5/20 Yukio Mituiwa.
writing
WARNING:This Line dosen't start with"S".
Address Size seems wrong
WARNING:This Line dosen't start with"S".
Address Size seems wrong
........
EEPROM Writing is successed.
$ cu -l /dev/cu.usbserial
Connected.
Hello world!
a
a
~.

Disconnected.
$
```

## ELF実行ファイルを読む

```bash
$ h8300-elf-readelf -a kzload.elf
ELF Header:
  Magic:   7f 45 4c 46 01 02 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF32

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .vectors          PROGBITS        00000000 000054 000100 00  WA  0   0  4
  [ 2] .text             PROGBITS        00000100 000154 000230 00  AX  0   0  2
  [ 3] .text.startup     PROGBITS        00000330 000384 00003c 00  AX  0   0  2
  [ 4] .rodata           PROGBITS        0000036c 0003c0 00002c 00   A  0   0  4
  [ 5] .comment          PROGBITS        00000000 0003ec 000011 01  MS  0   0  1
  [ 6] .symtab           SYMTAB          00000000 000400 0003c0 10      7  44  4
  [ 7] .strtab           STRTAB          00000000 0007c0 00015e 00      0   0  1
  [ 8] .shstrtab         STRTAB          00000000 00091e 000049 00      0   0  1

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000054 0x00000000 0x00000000 0x00398 0x00398 RWE 0x1

 Section to Segment mapping:
  Segment Sections...
   00     .vectors .text .text.startup .rodata

Symbol table '.symtab' contains 60 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
    42: 0000036c    12 OBJECT  LOCAL  DEFAULT    4 _regs
    45: 00000000   256 OBJECT  GLOBAL DEFAULT    1 _vectors
    46: 00000220    36 NOTYPE  GLOBAL DEFAULT    2 _putc
    47: 00000244    26 NOTYPE  GLOBAL DEFAULT    2 _puts
    59: 00000330    60 NOTYPE  GLOBAL DEFAULT    3 _main

No version information found in this file.
$
```

## 静的変数をRAMへ配置（未解決）

```bash
$ vi ld.scr

    ```patch
    diff --git a/bootload/ld.scr b/bootload/ld.scr
    index 1e7c06c..943e68e 100644
    --- a/bootload/ld.scr
    +++ b/bootload/ld.scr
    @@ -21,6 +21,8 @@ SECTIONS
             . = ALIGN(4);/* http://kozos.jp/books/makeos/index.html#binutils-new */
         }

    +    . = 0xffbf20;
    +
         .data : {
             *(.data)
         }
    ```

$ make
$ make write
../tools/h8write/h8write -3069 -f20 kzload.mot /dev/cu.usbserial
H8/3069F is ready!  2002/5/20 Yukio Mituiwa.
writing
WARNING:This Line dosen't start with"S".
Address Size seems wrong
WARNING:This Line dosen't start with"S".
Address is out of range. Skipping..            <= エラー
WARNING:This Line dosen't start with"S".
Address Size seems wrong
........
EEPROM Writing is successed.
$ cu -l /dev/cu.usbserial
Connected.
Hello world!
0        <= &value = 0xffbf20 はこの段階では未定義。
14       <= value = 20 (0x14) することで 0xffbf20 に20がセットされる
~.

Disconnected.
$ h8300-elf-readelf -a kzload.elf
ELF Header:
  Magic:   7f 45 4c 46 01 02 01 00 00 00 00 00 00 00 00 00

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .vectors          PROGBITS        00000000 000094 000100 00  WA  0   0  4
  [ 2] .text             PROGBITS        00000100 000194 000230 00  AX  0   0  2
  [ 3] .text.startup     PROGBITS        00000330 0003c4 00004e 00  AX  0   0  2
  [ 4] .rodata           PROGBITS        00000380 000414 00002c 00   A  0   0  4
  [ 5] .data             PROGBITS        00ffbf20 000440 000002 00  WA  0   0  2

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000094 0x00000000 0x00000000 0x0037e 0x0037e RWE 0x1
  LOAD           0x000414 0x00000380 0x00000380 0x0002c 0x0002c R   0x1
  LOAD           0x000440 0x00ffbf20 0x00ffbf20 0x00002 0x00002 RW  0x1

Symbol table '.symtab' contains 62 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
    43: 00000380    12 OBJECT  LOCAL  DEFAULT    4 _regs
    49: 00ffbf20     2 OBJECT  GLOBAL DEFAULT    5 _value
    61: 00000330    78 NOTYPE  GLOBAL DEFAULT    3 _main
```

## ステップ3の本作業

```bash
$ vi ld.scr
$ vi startup.c
$ vi main.c
$ make
/usr/local/h8/h8-elf/bin/h8300-elf-gcc vector.o startup.o main.o lib.o serial.o -o kzload -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -DKZLOAD -static -T ld.scr -L.
cp kzload kzload.elf
/usr/local/h8/h8-elf/bin/h8300-elf-strip kzload
$ h8300-elf-readelf -a kzload.elf

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .vectors          PROGBITS        00000000 000094 000100 00  WA  0   0  4
  [ 2] .text             PROGBITS        00000100 000194 00033c 00  AX  0   0  2
  [ 3] .rodata           PROGBITS        0000043c 0004d0 000080 00   A  0   0  4
  [ 4] .data             PROGBITS        00fffc20 000550 000004 00  WA  0   0  2
  [ 5] .bss              NOBITS          00fffc24 000554 000004 00  WA  0   0  2

Program Headers:
Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000094 0x00000000 0x00000000 0x00100 0x00100 RW  0x1
  LOAD           0x000194 0x00000100 0x00000100 0x003bc 0x003bc R E 0x1
  LOAD           0x000550 0x00fffc20 0x000004bc 0x00004 0x00008 RW  0x1

 Section to Segment mapping:
  Segment Sections...
   00     .vectors
   01     .text .rodata
   02     .data .bss

Symbol table '.symtab' contains 77 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
    12: 00fffc20     2 OBJECT  LOCAL  DEFAULT    4 _static_data
    13: 00fffc24     2 OBJECT  LOCAL  DEFAULT    5 _static_bss
    46: 0000043c    12 OBJECT  LOCAL  DEFAULT    3 _regs
    49: 00fffc26     2 OBJECT  GLOBAL DEFAULT    5 _global_bss
    50: 00000000   256 OBJECT  GLOBAL DEFAULT    1 _vectors
    51: 000002aa    36 NOTYPE  GLOBAL DEFAULT    2 _putc
    52: 0000043c     0 NOTYPE  GLOBAL DEFAULT    2 _etext
    53: 000002ce    26 NOTYPE  GLOBAL DEFAULT    2 _puts
    54: 00fffc24     0 NOTYPE  GLOBAL DEFAULT    5 _bss_start
    55: 00000100     0 NOTYPE  GLOBAL DEFAULT    2 _text_start
    57: 00fffc28     0 NOTYPE  GLOBAL DEFAULT    5 _ebss
    61: 00fffc20     0 NOTYPE  GLOBAL DEFAULT    4 _data_start
    65: 00fffc22     2 OBJECT  GLOBAL DEFAULT    4 _global_data
    68: 0000043c     0 NOTYPE  GLOBAL DEFAULT    3 _rodata_start
    69: 00fffc24     0 NOTYPE  GLOBAL DEFAULT    4 _edata
    70: 00fffc28     0 NOTYPE  GLOBAL DEFAULT    5 _end
    73: 00ffff00     0 NOTYPE  GLOBAL DEFAULT    6 _stack
    74: 000003ba   130 NOTYPE  GLOBAL DEFAULT    2 _main
    75: 000004bc     0 NOTYPE  GLOBAL DEFAULT    3 _erodata

$ make write
$ cu -l /dev/cu.usbserial
Connected.
Hello world!
global_data = 10
global_bss  = 0
static_data = 20
static_bss  = 0
overwrite variables.
global_data = 20
global_bss  = 30
static_data = 40
static_bss  = 50

```
