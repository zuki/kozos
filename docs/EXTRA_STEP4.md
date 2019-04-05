# 移植編実装その4: [(H8移植編その２第２回)タイマをキュー管理する](http://kozos.jp/kozos/h8_2_02.html)

## osの修正

```bash
$ vi timedrv.[hc]
$ vi clock.c
$ vi timer.[hc], command.c, main.c, defines.h, kozos.h, Makefile
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
Bytes Sent:  17664   BPS:1970

Transfer complete
                 XMODEM receive succeeded.
kzload> run
starting from entry point: ffc020
kozos boot succeed!
command> ready.
ready.
timerready.

timer start.
command> expired.
command> ready.
ready.
ready.
ready.
timer
timer start.
command> ready.
expired.
command> ready.
ready.
ready.
ready.
                   # <= リセットボタン押下
kzload (kozos boot loader) started.
kzload> ~
[EOT]
```
