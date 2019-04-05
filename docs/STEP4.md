# ステップ4

## ブートローダ実装

- check_sumの計算などで`unsigned char`が必要だったので、もとに戻した。
- Mac搭載のcuコマンドはuucp由来で`~C`内部コマンドがない。
- この内部コマンドがあるのはBSD系Unixのtip由来のcuコマンドであることが判明したので、NetBSDのtipコマンドをMac上でコンパイルして使用した。
- dumpコマンドで2ブロックになっているのは本にはないコメントをつけているため。

```bash
$ vi xmodem.h
$ vi xmodem.c
$ vi Mekfile, ld.scr, lib.h, lib.c, serial.h, serial.c
$ make
$ make write
$ /usr/local/bsd/bin/cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx defines.h
Sending defines.h, 2 blocks: Give your local XMODEM receive command now.
Bytes Sent:    384   BPS:122

Transfer complete

XMODEM receive succeeded.
kzload> dump
size: 180
23 69 66 6e 64 65 66 20  5f 44 45 46 49 4e 45 53
5f 48 5f 49 4e 43 4c 55  44 45 44 5f 0a 23 64 65
66 69 6e 65 20 5f 44 45  46 49 4e 45 53 5f 48 5f
49 4e 43 4c 55 44 45 44  5f 0a 0a 23 64 65 66 69
6e 65 20 4e 55 4c 4c 20  28 28 76 6f 69 64 20 2a
29 20 30 29 20 20 20 20  20 20 20 20 20 2f 2a 20
4e 55 4c 4c e3 83 9d e3  82 a4 e3 83 b3 e3 82 bf
e5 ae 9a e7 be a9 20 20  20 20 20 20 20 20 20 2a
2f 0a 23 64 65 66 69 6e  65 20 53 45 52 49 41 4c
5f 44 45 46 41 55 4c 54  5f 44 45 56 49 43 45 20
31 20 20 20 2f 2a 20 e6  a8 99 e6 ba 96 e3 82 b7
e3 83 aa e3 82 a2 e3 83  ab e3 83 87 e3 83 90 e3
82 a4 e3 82 b9 e7 95 aa  e5 8f b7 20 2a 2f 0a 0a
74 79 70 65 64 65 66 20  75 6e 73 69 67 6e 65 64
20 63 68 61 72 20 20 20  75 69 6e 74 38 3b 0a 74
79 70 65 64 65 66 20 75  6e 73 69 67 6e 65 64 20
73 68 6f 72 74 20 20 75  69 6e 74 31 36 3b 0a 74
79 70 65 64 65 66 20 75  6e 73 69 67 6e 65 64 20
6c 6f 6e 67 20 20 20 75  69 6e 74 33 32 3b 0a 0a
23 65 6e 64 69 66 0a 1a  1a 1a 1a 1a 1a 1a 1a 1a
1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a

kzload> ~.
```

## アセンブラ

- 最適化パラメタを変えると結果が変わる。`-Os`ではスタックフレームを使用していない。

```
$ vi lib.h, lib.c, main.c
$ make
$ /usr/local/h8/h8-elf/h8300-elf-objdump -d kzload.elf

# -Os
00000504 <_main>:
 542:   79 01 00 02     mov.w   #0x2,r1
 546:   79 00 00 01     mov.w   #0x1,r0
 54a:   5e 00 03 1a     jsr @0x31a:24

0000031a <_func>:
 31a:   1b 97           subs    #4,er7
 31c:   09 10           add.w   r1,r0
 31e:   6f f0 00 02     mov.w   r0,@(0x2:16,er7)
 322:   6f 70 00 02     mov.w   @(0x2:16,er7),r0
 326:   0b 97           adds    #4,er7
 328:   54 70           rts

# -O0
0000027e <_main>:
 288:   79 01 00 02     mov.w   #0x2,r1
 28c:   79 00 00 01     mov.w   #0x1,r0
 290:   5e 00 09 0c     jsr @0x90c:24

0000090c <_func>:
 90c:   01 00 6d f6     mov.l   er6,@-er7
 910:   0f f6           mov.l   er7,er6
 912:   7a 37 00 00     sub.l   #0x8,er7
 916:   00 08
 918:   6f e0 ff fa     mov.w   r0,@(0xfffa:16,er6)
 91c:   6f e1 ff f8     mov.w   r1,@(0xfff8:16,er6)
 920:   6f 63 ff fa     mov.w   @(0xfffa:16,er6),r3
 924:   6f 62 ff f8     mov.w   @(0xfff8:16,er6),r2
 928:   09 32           add.w   r3,r2
 92a:   6f e2 ff fe     mov.w   r2,@(0xfffe:16,er6)
 92e:   6f 62 ff fe     mov.w   @(0xfffe:16,er6),r2
 932:   0d 20           mov.w   r2,r0
 934:   0b 97           adds    #4,er7
 936:   0b 97           adds    #4,er7
 938:   01 00 6d 76     mov.l   @er7+,er6
 93c:   54 70           rts
 ```

### -O0のfunc()のスタックフレーム

```
 X0000100  <= return address             |  938: => er7
 X00000fc  <= er6                        |  935: => er7
 X00000fa  <= c = @(0xfffe:16,er6)       |
 X00000f8  <=                            |  934: => er7
 X00000f6  <= a = @(0xfffa:16,er6)       |
 X00000f4  <= b = @(0xfff8:16,er6), er7  |
 X00000f0
 ```
