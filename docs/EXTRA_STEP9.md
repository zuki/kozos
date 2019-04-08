# 移植編実装その9: [(H8移植編その２第７回)](http://kozos.jp/kozos/h8_2_07.html)と[(H8移植編その２第８回)](http://kozos.jp/kozos/h8_2_08.html)

これら2回分の修正を適用した。ただし、第7回のROM化は行っていない。

## osの編集

```bash
$ vi memory.c, netdrv.c, tcp.c
$ make clean && make && make image
```

## 実行

**転送途中でブラウザを停止すると、以降接続できなくなる症状がある。LED1が定期的に点滅するので、httpdが切れていない？のではないか。**

```bash
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx kozos.mot
Sending kozos.mot, 538 blocks: Give your local XMODEM receive command now.
Bytes Sent:  68992   BPS:3501

Transfer complete
                 XMODEM receive succeeded.
kzload> run
starting from entry point: 400100
kozos boot succeed!
command> MAC: 0002cb045d12

unknown.
command> ~
[EOT]
```
