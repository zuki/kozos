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
