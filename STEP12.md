# ステップ12

# osの修正

```bash
$ vi consdrv.[hc]
$ vi command.c
$ vi defines.h, kozos.[hc], syscall.[ch], main.c, Makefile
$ make
```

## 実行

```bash
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx kozos
Sending kozos, 43 blocks: Give your local XMODEM receive command now.
Bytes Sent:   5504   BPS:498

Transfer complete

XMODEM receive succeeded.
kzload> run
starting from entry point: ffc020
kozos boot succeed!
command> echo aaa
 aaa
command> ~
[EOT]
```
