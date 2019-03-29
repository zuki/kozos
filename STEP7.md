# ステップ6

## kzloaderの修正

```bash
$ vi intr.h
$ vi intr.S
$ vi interrupt.h
$ vi interrupt.c
$ vi ld.scr, vector.c, main.c, Makefile
$ make
```

## osの修正

```bash
$ cp ../bootload/intr.h .
$ cp ../bootload/interrupt.[hc]
$ vi ld.scr, serial.[hc], main.c, Makefile
$ make
```

## 実行

```
$ cd bootload
$ make write
$ cd ../os
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx kozos
Sending kozos, 13 blocks: Give your local XMODEM receive command now.
Bytes Sent:   1792   BPS:272

Transfer complete

XMODEM receive succeeded.
kzload> run
starting from entry point: ffc020
kozos boot succeed!
> echo test
 test
> ~.
[EOT]
```
