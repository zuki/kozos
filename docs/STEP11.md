# ステップ11

## osの修正

```bash
$ vi test11_[12].c
$ vi defines.h, syscall.[hc], kozos.[hc], main.c, Makeffile
$ make
```
## 実行

```bash
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx kozos
Sending kozos, 38 blocks: Give your local XMODEM receive command now.
Bytes Sent:   4992   BPS:504

Transfer complete

XMODEM receive succeeded.
kzload> run
starting from entry point: ffc020
kozos boot succeed!
test11_1 started.
test11_1 recv in.
test11_2 started.
test11_2 send in.
test11_1 recv out.
static memory
test11_1 recv in.
test11_2 send out.
test11_2 send in.
test11_1 recv out.
allocated memory
test11_1 send in.
test11_1 send out.
test11_1 send in.
test11_1 send out.
test11_1 exit.
test11_1 EXIT.
test11_2 send out.
test11_2 recv in.
test11_2 recv out.
static memory
test11_2 recv in.
test11_2 recv out.
allocated memory
test11_2 exit.
test11_2 EXIT.
~.
[EOT]
```
