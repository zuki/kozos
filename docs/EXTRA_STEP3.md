# 移植編実装その3: [(H8移植編その２第１回)タイマを動かしてみよう](http://kozos.jp/kozos/h8_2_01.html)

## 作業メモ

- タイマ関係の割込ベクタアドレスは、ハードウェアマニュアルの`5.3.3`。
- タイマ関係のレジスタ構成は、ハードウェアマニュアルの`10.1.4`。

## bootloadの修正

```bash
$ vi intr.S, intr.h, vector.c
$ make clean && make
$ make write
```

## osの修正

```bash
$ vi timer.[hc]
$ vi command.c, intr.h, Makefile
$ make clean && make
```

## 実行

```bash
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx kozos.mot
Sending kozos.mot, 124 blocks: Give your local XMODEM receive command now.
Bytes Sent:  16000   BPS:1820

Transfer complete
                 XMODEM receive succeeded.
kzload> run
starting from entry point: ffc020
kozos boot succeed!
command> echo sample
 sample
command> timer
start timer.
command> timer expired 0.
timer expired 1.
timer expired 0.
timer expired 0.
timer expired 0.
timer expired 0.
cancel
cancel timer.
command> ~
[EOT]
```
