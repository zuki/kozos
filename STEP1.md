# ステップ1

## 環境構築

- Mac mini 2.8GHz Intel Core i5
- macOS Mojave (10.14.3)

    ```bash
    $ cc -v
    Apple LLVM version 10.0.0 (clang-1000.11.45.5)
    Target: x86_64-apple-darwin18.2.0
    Thread model: posix
    ```

- gcc-8.3.0 (binutils-2.32, gmp-6.1.2, mpfr-4.0.2, mpc-1.1.0)

    gcc等のインストール法についてはバージョンは違うが、[ここ](http://d.hatena.ne.jp/satfy/20101226/1293370919)に書かれているとおり。ただし、`prefix`を**gcc**と**binutils**は`/usr/local/h8/h8-elf`、その他は`/usr/local/h8/[gmp|mpfr|mpc]`とした。

- [USBシリアル変換ケーブル](http://akizukidenshi.com/catalog/faq/goodsfaq.aspx?goods=M-02746)で接続（延長ケーブルがないとマイコンに接続できないことを後で知り、追加購入）
- USBシリアルドライバは[Prolificのサイト](http://www.prolific.com.tw/US/ShowProduct.aspx?p_id=229&pcid=41)から**PL2303**用の[Mac用最新版](http://www.prolific.com.tw/UserFiles/files/PL2303_MacOSX_1_6_1_20171018.zip)をダウンロードしてインストール。
- `/dev/cu.usbserial`で認識した。

    ```bash
    $ ls -l /dev/cu.usbserial
    crw-rw-rw-  1 root  wheel   18,   3  3 24 15:46 /dev/cu.usbserial
    ```

## ステップ1の作業

### 1. Makefileを環境に合わせて変更

```bash
$ cd kozos/bootload
$ vi Makefile
    PREFIX  = /usr/local/h8/h8-elf
    H8WRITE_SERDEV = /dev/cu.usbserial
```

### 2. h8writeを用意

```bash
$ mkdir kozos/tools/h8write
$ cd kozos/tools/h8write
$ wget http://mes.osdn.jp/h8/h8write.c
$ cc -o h8write h8write.c
```

### 3. bootloadのmake

```bash
$ cd kozos/bootloader
$ vi *.[h*], Makefile, ld.scr
$ make
/usr/local/h8/h8-elf/bin/h8300-elf-gcc -c -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -DKZLOAD vector.c
/usr/local/h8/h8-elf/bin/h8300-elf-gcc -c -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -DKZLOAD startup.s
/usr/local/h8/h8-elf/bin/h8300-elf-gcc -c -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -DKZLOAD main.c
main.c: In function 'main':
main.c:9:8: warning: pointer targets in passing argument 1 of 'puts' differ in signedness [-Wpointer-sign]
puts("Hello world!\n");
     ^~~~~~~~~~~~~~~~
In file included from main.c:3:
lib.h:5:5: note: expected 'unsigned char *' but argument is of type 'char *'
int puts(unsigned char *str);   /* 文字列送信 */
    ^~~~
/usr/local/h8/h8-elf/bin/h8300-elf-gcc -c -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -DKZLOAD lib.c
/usr/local/h8/h8-elf/bin/h8300-elf-gcc -c -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -DKZLOAD serial.c
/usr/local/h8/h8-elf/bin/h8300-elf-gcc vector.o startup.o main.o lib.o serial.o -o kzload -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -DKZLOAD -static -T ld.scr -L.
cp kzload kzload.elf
/usr/local/h8/h8-elf/bin/h8300-elf-strip kzload

$ ls -F
Makefile	ld.scr		main.c		serial.o	vector.o
defines.h	lib.c		main.o		startup.o
kzload*		lib.h		serial.c	startup.s
kzload.elf*	lib.o		serial.h	vector.c
```

### 4. bootloader書き込み

1. DIPスイッチをセット(on, on, off, on)
2. シリアル線接続
3. 電源オン
4. 書き込み処理（一般ユーザにも/dev/cu.usbserialにrw権があるので一般ユーザで実行できる）

```bash
$ make write
../tools/h8write/h8write -3069 -f20 kzload.mot /dev/cu.usbserial
H8/3069F is ready!  2002/5/20 Yukio Mituiwa.
writing
WARNING:This Line dosen't start with"S".
Address Size seems wrong
WARNING:This Line dosen't start with"S".
Address Size seems wrong
....
EEPROM Writing is successed.
```

### 5. 実行

1. DIPスイッチをセット(on, off, on, off)
2. シリアル接続（一般ユーザにも/dev/cu.usbserialにrw権があるので一般ユーザで実行できる）

```bash
$ cu -l /dev/cu.usbserial
Connected.                  # リセットスイッチ・オン
Hello world!                # リセットスイッチ・オン
Hello world!
```
