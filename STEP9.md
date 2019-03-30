# ステップ9

## osの修正

```bash
$ vi test09_1.c, test09_2.c, test09_3
$ vi kosos.[hc], syscall.[hc], main.c, Makefile
$ make
```

## 実行

```bash
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
unknown.
kzload> load
~CLocal command? lsx kozos
Sending kozos, 32 blocks: Give your local XMODEM receive command now.
Bytes Sent:   4224   BPS:636

Transfer complete

XMODEM receive succeeded.
kzload> run
starting from entry point: ffc020
kozos boot succeed!
test09_1 started.
test09_1 sleep in.
test09_2 started.
test09_2 sleep in.
test09_3 started.
test09_3 wakeup in (test09_1).
test09_1 sleep out.
test09_1 chpri in.
test09_3 wakeup out.
test09_3 wakeup in (test09_2).
test09_2 sleep out.
test09_2 chpri in.
test09_1 chpri out.
test09_1 wait in.
test09_3 wakeup out.
test09_3 wait in.
test09_2 chpri out.
test09_2 wait in.
test09_1 wait out.
test09_1 trap in.
test09_1 DOWN.
test09_1 EXIT.
test09_3 wait out.
test09_3 exit in.
test09_3 EXIT.
test09_2 wait out.
test09_2 exit.
test09_2 EXIT.
~.
[EOT]
```
