# 移植編実装その11: [(H8移植編その２第11回)TCPの再送処理の実装準備その２](http://kozos.jp/kozos/h8_2_11.html)と[(H8移植編その２第12回)ethernetドライバのバグ修正](http://kozos.jp/kozos/h8_2_12.html)の適用

## osの編集

```bash
$ vi lib.c, rtl8019.c, tcp.c
$ make clean && make && make image
```

## 実行

ウェブブラウザおよびtelnetによる接続で問題なし。
