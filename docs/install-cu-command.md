# cuコマンドのインストール

macには標準で`cu`コマンドが存在するが、これは`uucp`由来で、BSD系Unixの`tip`由来の`cu`コマンで使用できる`~C`内部コマンドが存在しない。そのため、BSD系Unixから`tip`コマンドのソースを取得し、コンパイルして使用する。

## 1. bsdmakeコマンドのインストール

BSD系Unixのソースをコンパイルするために`bsdmake`コマンドを`Homebrew`からインストールする。

```bash
$ brew intall bsdmake
$ which bsdmake
/usr/local/bin/bsdmake
```

## 2. tipコマンドのインストール

NetBSDのrelease版のソースをダウンロードしてbsdmakeする。closefromシステムコールがMacにはないため、代替ソースを`https://github.com/practicalswift/osx`からダウンロードして使用する。

```bash
$ mkdir netbsd-src && cd netbsd-src
$ ftp -i ftp://ftp.NetBSD.org/pub/NetBSD/NetBSD-8.0/source/sets/
ftp> mget src.tgz
ftp> quit
$ tar xf src.tgz

# closefromのソースをダウンロード、makeしてオブジェクトファイルをコピー
$ wget https://github.com/practicalswift/osx/raw/master/tarballs/s/sudo-86_2017-09-26_macos-1013_f36a47df.tar.gz
$ tar xf sudo-86_2017-09-26_macos-1013_f36a47df.tar.gz
$ cd sudo-86/sudo/lib/util
$ make closefrom.lo
$ make strtonum.lo
$ cp .libs/closefrom.o .libs/strtonum.o netbsd-src/usr/src/usr.bin/tip

$ cd netbsd-src/usr/src/usr.bin/tip
$ vi Makefile
@@ -33,6 +33,13 @@ MLINKS=  tip.1 cu.1
 CPPFLAGS+=-I${.CURDIR} \
    -DDEFBR=9600 -DDEFFS=BUFSIZ -DHAYES \
    #-DV831 -DVENTEL -DHAYES -DCOURIER -DT3000
+CFLAGS+=-DDEFBR=9600 -DDEFFS=BUFSIZ -DHAYES -DHAVE_SNPRINTF \
+   -Wno-nullability-completeness -Wno-implicit-function-declaration \
+   -Wno-strict-prototypes -Wno-missing-prototypes \
+   -Wno-incompatible-pointer-types-discards-qualifiers \
+   -I${.CURDIR} -I/Users/dspace/Downloads/sudo-86/sudo \
+   -I/Users/dspace/Downloads/sudo-86/sudo/include
+
 .PATH: ${.CURDIR}/aculib

 # Dialers we can choose from:
@@ -42,6 +49,8 @@ DIALSRCS = hayes.c
 SRCS=  acu.c acutab.c cmds.c cmdtab.c cu.c hunt.c partab.c \
    remote.c tip.c tipout.c value.c vars.c ${DIALSRCS}

+OBJS+=closefrom.o strtonum.o
+
 # -- acutab is configuration dependent, and so depends on the Makefile
 # -- remote.o depends on the Makefile because of DEFBR and DEFFS
 acutab.o remote.o: Makefile

$ vi cmds.c
@@ -40,6 +40,7 @@ __RCSID("$NetBSD: cmds.c,v 1.37 2014/07/12 05:28:07 mlelstv Ex
p $");
 #include "tip.h"
 #include "pathnames.h"

+#include "sudo_compat.h"
 /*
  * tip
  *

$ bsdmake
$ sudo mkdir -p /usr/local/bsd/bin
$ sudo BINDIR=/usr/local/bsd/bin bsdmake install
$ echo "PATH=/usr/local/bsd/bin:$PATH" >> ~/.bashrc
$ source ~/.bashrc
$ which cu
/usr/local/bsd/bin/cu
```

## 3. 使用する

```
$ echo "alias cu='cu -s 38400'" >> ~/.bashrc
$ source ~/.bashrc
$ cd kozos/os
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx kozos
Sending kozos, 203 blocks: Give your local XMODEM receive command now.
Bytes Sent:  26112   BPS:2605

Transfer complete
                 Elf loading succeeded.
kzload> run
starting from entry point: 400100
kozos boot succeed!
command> MAC: 0002cb045d12

unknown.
command> ping
ping start.
command> ICMP received: c0a80b07 00 00 08ec
ICMP received: c0a80b07 00 00 08eb
ICMP received: c0a80b07 00 00 08ea
ICMP received: c0a80b07 08 00 8574
ICMP received: c0a80b07 08 00 70ff
ICMP received: c0a80b07 08 00 5c33

unknown.
command> ~
[EOT]
```
