# ステップ6

## kzloaderの修正

```bash
$ vi ld.scr, intr.S, startup.s
$ make
```

## osの修正

```bash
$ vi id.scr, startup.s, defines.h, main.c, Makefile
$ vi kosos.[hc], syscall.[hc], test08_1.c
$ make
```

## os make時のエラー

```
/usr/local/h8/h8-elf/bin/h8300-elf-gcc startup.o main.o interrupt.o lib.o serial.o kozos.o syscall.o test08_1.o -o kozos -Wall -mh -nostdinc -nostdlib -fno-builtin -I. -Os -Wno-pointer-sign -DKOZOS -static -T ld.scr -L.
/usr/local/h8/h8-elf/lib/gcc/h8300-elf/8.3.0/../../../../h8300-elf/bin/ld: kozos.o: in function `.L13':
kozos.c:(.text+0x135): undefined reference to `___mulsi3'
/usr/local/h8/h8-elf/lib/gcc/h8300-elf/8.3.0/../../../../h8300-elf/bin/ld: kozos.c:(.text+0x149): undefined reference to `___mulsi3'
/usr/local/h8/h8-elf/lib/gcc/h8300-elf/8.3.0/../../../../h8300-elf/bin/ld: kozos.c:(.text+0x163): undefined reference to `___mulsi3'
/usr/local/h8/h8-elf/lib/gcc/h8300-elf/8.3.0/../../../../h8300-elf/bin/ld: kozos.c:(.text+0x179): undefined reference to `___mulsi3'
/usr/local/h8/h8-elf/lib/gcc/h8300-elf/8.3.0/../../../../h8300-elf/bin/ld: kozos.c:(.text+0x1bb): undefined reference to `___mulsi3'
/usr/local/h8/h8-elf/lib/gcc/h8300-elf/8.3.0/../../../../h8300-elf/bin/ld: kozos.o:kozos.c:(.text+0x217): more undefined references to `___mulsi3' follow
collect2: error: ld returned 1 exit status
make: *** [kozos] Error 1
```

- `kz_thread`の長さが2のべき乗でないため。
- 定義では**48**なので、16バイトを足して、64 = 2^6 とする。

    ```c
    struct _kz_thread *next;                =  4
    char name[THREAD_NAME_SIZE + 1];        = 16
    char *stack;                            =  4

    struct {
        kz_func_t func;                     =  4
        int argc;                           =  2
                      padding               =  2
        char **argv;                        =  4
    } init;                                 = 12

    struct {
      kz_syscall_type_t type;               =  2
                      padding               =  2
      kz_syscall_param_t *param;            =  4
    } syscall;                              =  8

    kz_context context;                     =  4
                                            = 48
    char dummy[16]                          = 16    <= これを追加
                                            = 64
    ```

## 実行

```bash
$ cd bootload
$ make write
$ cd os
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx kozos
Sending kozos, 22 blocks: Give your local XMODEM receive command now.
Bytes Sent:   2944   BPS:565

Transfer complete

XMODEM receive succeeded.
kzload> run
starting from entry point: ffc020
kozos boot succeed!
start EXIT.
est08_1 started.
> echo test
 test
> exit
test08_1 exit.
command EXIT.
system error!
~.
[EOT]
```
