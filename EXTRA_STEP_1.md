# 移植編実装その１: [(H8移植編第10回)モトローラSレコードフォーマット対応](http://kozos.jp/kozos/h8_10.html)

## 成功に至るまでに発生した問題

### 1. `lsx kozos.mot` （xmodem通信）した際に、次のエラーが発生し処理が止まる。

    ```bash
    Retry 0: NAK on sector
    Retry 0: Got 0d for sector ACK
    ```

- これは`xmodem_recv()`の1回めの受信がエラーとなり、NAKを送信して2回目の受信を行った際に、`if`条件に合致するデータが得られず、まだ、lsxが終了しないうちに`xmodem_recv()`から抜けてしまい、リターン先の`main()`において次の行の`puts("\nXMODEMO receive error.\n")`が実行され、これがlsxに送られていた（`\n`が送信時に`\r\n`に変換されるため、`0x0d`が不正文字になる）ことが判明した。
- 1回目の受信でエラーになるのは、Sレコードフォーマットのチェックサム不正が原因であった。しかし、実際の受信データで検証した結果、チェックサムは正しかった。
- 文字をすべて`unsigned char`とし、チェックサムを計算する際には`& 0xff`することで正しく計算されるようになった（たぶん）。

#### 作業メモ

1. Homebrewからインストールした`lrzsz`を削除し、ソースにデバッグ出力を入れ、コンパイルして使用した。
2. bzload側のデバッグ方法がわからなくて困った。`main()`で`puts()`した文字列がlsxに送られるので、`xmodem.c`と`srec.c`でエラーが発生した場合、全て別の数字を返すようにして、その数字を`puts()`することで、どこで問題が発生しているのかを確認していった。

### 2. kozosを`ffc020`に転送するにしたが、先頭バイトが別のコードで上書きされており、正常に実行できない

    ```
    [ffc020からdumpした結果]
    1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
    1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
    1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
    1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
    1a 1a 1a 66 79 02 00 01  7a 01 00 ff d3 58 7a 00
    00 ff d0 f4 5e ff cb 9a  01 00 6d f4 1b 97 1b 97
    6f f4 00 06 6f f5 00 02  79 02 00 08 7a 01 00 ff
    d3 60 7a 00 00 ff d2 2c  5e ff cb 9a 79 00 00 0f
    ```

- kozosを逆アセンブルしたところ、`0xffc064`からは正しいコードであることがわかった。
- `ld.scr`を確認したところ、上書き部分は`kzload`のデータの一部であった。
- つまり、リンクスクリプトの設定ミスである（理解が十分でなかった）ことが判明した。

## bootloadの変更

```bash
$ vi srec.[hc]
$ vi xmodem.[hc], main.c, ld.scr, Makefile
$ make
```

## osの変更

```bash
$ vi ld.scr, main.c, Makefile
$ make
$ make image
```

## 実行

```
$ cd bootload
$ make write
$ cd ../os
$ cu -l /dev/cr.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx kozos.mot
Sending kozos.mot, 114 blocks: Give your local XMODEM receive command now.

    [lsx デバッグ出力開始]
    53 30 30 43 30 30 30 30  36 42 36 46 37 41 36 46
    37 33 32 45 36 44 36 46  37 34 33 46 0d 0a 53 32
    31 34 46 46 43 30 32 30  37 41 30 37 30 30 46 46
    46 46 30 30 35 45 46 46  44 32 43 36 34 30 46 45
    30 31 30 30 36 39 30 37  45 39 0d 0a 53 32 31 34
    46 46 43 30 33 30 30 31  30 30 36 44 37 30 30 31
    30 30 36 44 37 31 30 31  30 30 36 44 37 32 30 31
    30 30 36 44 37 33 37 45  0d 0a 53 32 31 34 46 46

    wcputsec start: sectnum: 1, seclen: 128
    Xmodem sectors/kbytes sent:   0/ 0k
    attempt: 1
    0101feS00C00006B6F7A6F732E6D6F743F
    S214FFC0207A0700FFFF005EFFD2C640FE01006907E9
    S214FFC03001006D7001006D7101006D7201006D737E
    S214FF65
    send xmodem line
    firstch: 0006
    get ACK
    [01] send ok.

    [省略]

    30 30 30 30 30 30 30 30  44 31 0d 0a 53 38 30 34
    46 46 43 30 32 30 31 43  0d 0a 1a 1a 1a 1a 1a 1a
    1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
    1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
    1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
    1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
    1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a
    1a 1a 1a 1a 1a 1a 1a 1a  1a 1a 1a 1a 1a 1a 1a 1a

    wcputsec start: sectnum: 115, seclen: 128
    Xmodem sectors/kbytes sent: 114/14k
    attempt: 1
    01738c00000000D1
    S804FFC0201C
    43
    send xmodem line
    firstch: 0006
    get ACK
    [115] send ok.

    [lsx デバッグ出力終了]

Bytes Sent:  14720   BPS:752

Transfer complete
                 XMODEM receive succeeded.
load> dump
size: 3980
starting from entry point: ffc020
7a 07 00 ff ff 00 5e ff  d2 c6 40 fe 01 00 69 07
01 00 6d 70 01 00 6d 71  01 00 6d 72 01 00 6d 73

[省略]

0c 1b 04 cb 5f 6e 37 f7  68 92 59 47 de 2d ef fb
cf 12 19 49 64 93 47 bd  6a 52 f3 88 f7 2a 7b 74

kzload> run
starting from entry point: ffc020
kozos boot succeed!
command> echo aaa
 aaa
command> ~.
```
