# ステップ3

## list.3.1

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

## readelf

```bash
$ h8300-elf-readelf -a kzload.elf
ELF Header:
  Magic:   7f 45 4c 46 01 02 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF32
  Data:                              2's complement, big endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           Renesas H8/300
  Version:                           0x1
  Entry point address:               0x100
  Start of program headers:          52 (bytes into file)
  Start of section headers:          2408 (bytes into file)
  Flags:                             0x810000
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         1
  Size of section headers:           40 (bytes)
  Number of section headers:         9
  Section header string table index: 8

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
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  p (processor specific)

There are no section groups in this file.

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000054 0x00000000 0x00000000 0x00398 0x00398 RWE 0x1

 Section to Segment mapping:
  Segment Sections...
   00     .vectors .text .text.startup .rodata

There is no dynamic section in this file.

There are no relocations in this file.

The decoding of unwind sections for machine type Renesas H8/300 is not currently supported.

Symbol table '.symtab' contains 60 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
     0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND
     1: 00000000     0 SECTION LOCAL  DEFAULT    1
     2: 00000100     0 SECTION LOCAL  DEFAULT    2
     3: 00000330     0 SECTION LOCAL  DEFAULT    3
     4: 0000036c     0 SECTION LOCAL  DEFAULT    4
     5: 00000000     0 SECTION LOCAL  DEFAULT    5
     6: 00000000     0 FILE    LOCAL  DEFAULT  ABS vector.c
     7: 00000000     0 FILE    LOCAL  DEFAULT  ABS startup.o
     8: 0000010a     0 NOTYPE  LOCAL  DEFAULT    2 .L1^B1
     9: 00000000     0 FILE    LOCAL  DEFAULT  ABS main.c
    10: 0000036a     0 NOTYPE  LOCAL  DEFAULT    3 .L2
    11: 00000000     0 FILE    LOCAL  DEFAULT  ABS lib.c
    12: 0000011a     0 NOTYPE  LOCAL  DEFAULT    2 .L2
    13: 00000116     0 NOTYPE  LOCAL  DEFAULT    2 .L3
    14: 00000138     0 NOTYPE  LOCAL  DEFAULT    2 .L5
    15: 00000132     0 NOTYPE  LOCAL  DEFAULT    2 .L6
    16: 00000162     0 NOTYPE  LOCAL  DEFAULT    2 .L8
    17: 0000015e     0 NOTYPE  LOCAL  DEFAULT    2 .L9
    18: 0000016e     0 NOTYPE  LOCAL  DEFAULT    2 .L12
    19: 00000172     0 NOTYPE  LOCAL  DEFAULT    2 .L7
    20: 0000014e     0 NOTYPE  LOCAL  DEFAULT    2 .L11
    21: 0000017a     0 NOTYPE  LOCAL  DEFAULT    2 .L14
    22: 00000192     0 NOTYPE  LOCAL  DEFAULT    2 .L17
    23: 000001c2     0 NOTYPE  LOCAL  DEFAULT    2 .L20
    24: 000001d8     0 NOTYPE  LOCAL  DEFAULT    2 .L23
    25: 000001dc     0 NOTYPE  LOCAL  DEFAULT    2 .L19
    26: 000001b4     0 NOTYPE  LOCAL  DEFAULT    2 .L22
    27: 00000200     0 NOTYPE  LOCAL  DEFAULT    2 .L26
    28: 000001fc     0 NOTYPE  LOCAL  DEFAULT    2 .L27
    29: 00000212     0 NOTYPE  LOCAL  DEFAULT    2 .L31
    30: 0000021a     0 NOTYPE  LOCAL  DEFAULT    2 .L25
    31: 00000208     0 NOTYPE  LOCAL  DEFAULT    2 .L29
    32: 00000218     0 NOTYPE  LOCAL  DEFAULT    2 .L32
    33: 000001ee     0 NOTYPE  LOCAL  DEFAULT    2 .L30
    34: 00000234     0 NOTYPE  LOCAL  DEFAULT    2 .L34
    35: 00000252     0 NOTYPE  LOCAL  DEFAULT    2 .L36
    36: 0000024c     0 NOTYPE  LOCAL  DEFAULT    2 .L37
    37: 0000027a     0 NOTYPE  LOCAL  DEFAULT    2 .L39
    38: 000002a4     0 NOTYPE  LOCAL  DEFAULT    2 .L40
    39: 000002a2     0 NOTYPE  LOCAL  DEFAULT    2 .L41
    40: 00000288     0 NOTYPE  LOCAL  DEFAULT    2 .L42
    41: 00000000     0 FILE    LOCAL  DEFAULT  ABS serial.c
    42: 0000036c    12 OBJECT  LOCAL  DEFAULT    4 _regs
    43: 00000318     0 NOTYPE  LOCAL  DEFAULT    2 .L4
    44: 000001a8    62 NOTYPE  GLOBAL DEFAULT    2 _strcmp
    45: 00000000   256 OBJECT  GLOBAL DEFAULT    1 _vectors
    46: 00000220    36 NOTYPE  GLOBAL DEFAULT    2 _putc
    47: 00000244    26 NOTYPE  GLOBAL DEFAULT    2 _puts
    48: 000002ee    26 NOTYPE  GLOBAL DEFAULT    2 _serial_is_send_enable
    49: 00000128    30 NOTYPE  GLOBAL DEFAULT    2 _memcpy
    50: 0000010c    28 NOTYPE  GLOBAL DEFAULT    2 _memset
    51: 00000100     0 FUNC    GLOBAL DEFAULT    2 _start
    52: 000002c2    44 NOTYPE  GLOBAL DEFAULT    2 _serial_init
    53: 0000025e   100 NOTYPE  GLOBAL DEFAULT    2 _putxval
    54: 0000018c    28 NOTYPE  GLOBAL DEFAULT    2 _strcpy
    55: 00000146    50 NOTYPE  GLOBAL DEFAULT    2 _memcmp
    56: 000001e6    58 NOTYPE  GLOBAL DEFAULT    2 _strncmp
    57: 00000178    20 NOTYPE  GLOBAL DEFAULT    2 _strlen
    58: 00000308    40 NOTYPE  GLOBAL DEFAULT    2 _serial_send_byte
    59: 00000330    60 NOTYPE  GLOBAL DEFAULT    3 _main

No version information found in this file.
$
```

##

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

## ステップ2

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
