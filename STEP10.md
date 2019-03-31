# ステップ10

## osの修正

図10.5の`mp->next`にアドレスが設定されるタイミングがまだ理解できない。

```bash
$ vi memory.[hc]
$ vi test10_1.c
$ ld.scr, syscall.[hc], kozos.[hc], main.c, Makefile
$ make
```

## 実行

`freearea`は**0x00ffcfc0**。`kz_kmalloc`が返すアドレスはメモリヘッダの後のデータ領域のアドレスなので、アドレス最後の4bitは**8**となっている。

```bash
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx kozos
Sending kozos, 30 blocks: Give your local XMODEM receive command now.
Bytes Sent:   3968   BPS:521

Transfer complete

XMODEM receive succeeded.
kzload> run
starting from entry point: ffc020
kozos boot succeed!
test10_1 started.
00ffcfc8 aaa
00ffcfd8 bbb
00ffcfd8 aaaaaaa
00ffcfc8 bbbbbbb
00ffd048 aaaaaaaaaaa
00ffd068 bbbbbbbbbbb
00ffd068 aaaaaaaaaaaaaaa
00ffd048 bbbbbbbbbbbbbbb
00ffd048 aaaaaaaaaaaaaaaaaaa
00ffd068 bbbbbbbbbbbbbbbbbbb
00ffd068 aaaaaaaaaaaaaaaaaaaaaaa
00ffd048 bbbbbbbbbbbbbbbbbbbbbbb
00ffd148 aaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffd188 bbbbbbbbbbbbbbbbbbbbbbbbbbb
00ffd188 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffd148 bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
00ffd148 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffd188 bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
00ffd188 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffd148 bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
00ffd148 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffd188 bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
00ffd188 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffd148 bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
00ffd148 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffd188 bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
00ffd188 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
00ffd148 bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
test10_1 exit.
test10_1 EXIT.
~
[EOT]
```
