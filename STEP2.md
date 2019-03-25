# ステップ2

## ソースの修正

- `lic.h`, `lib.h`, `main.c`
- lib.cの出力関数の引数を`unsigned char*`から`char *`に変更した。ここで**unsigned**にする意味があるのかわからないが、問題があったら再度変更する。

## 実行

```bash
$ make
/usr/local/h8/h8-elf/bin/h8300-elf-gcc -c -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -DKZLOAD lib.c
/usr/local/h8/h8-elf/bin/h8300-elf-gcc vector.o startup.o main.o lib.o serial.o -o kzload -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -DKZLOAD -static -T ld.scr -L.
cp kzload kzload.elf
/usr/local/h8/h8-elf/bin/h8300-elf-strip kzload
$ make image
/usr/local/h8/h8-elf/bin/h8300-elf-objcopy -O srec kzload kzload.mot
$ make write
../tools/h8write/h8write -3069 -f20 kzload.mot /dev/cu.usbserial
H8/3069F is ready!  2002/5/20 Yukio Mituiwa.
writing
WARNING:This Line dosen't start with"S".
Address Size seems wrong
WARNING:This Line dosen't start with"S".
Address Size seems wrong
........
EEPROM Writing is successed.
$ cu -l /dev/cu.usbserial
Connected.
Hello world!
10
ffff
~.

Disconnected.
$
```
