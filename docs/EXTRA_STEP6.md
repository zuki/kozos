# 移植編実装その6: [(H8移植編その２第４回)ネットワークに接続しよう](http://kozos.jp/kozos/h8_2_04.html)

## bootloadの修正

```bash
$ vi intr.S, intr.h, vector.c
$ make clean && make
$ make write
```

## osの修正

```bash
$ vi ether.[hc]
$ vi etherdrv.[hc]
$ vi ip.c
$ vi intr.h, kozos.h, memory.c, main.c, defines.h, Makefile
$ make clean && make
```

### マイコン側

```bash
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> o
unknown.
kzload> load
~CLocal command? lsx kozos.mot
Sending kozos.mot, 181 blocks: Give your local XMODEM receive command now.
Bytes Sent:  23296   BPS:3509

Transfer complete
                 XMODEM receive succeeded.
kzload> run
starting from entry point: 400100
kozos boot succeed!
command> MAC: 0002cb045d12
network ready.
received: 0x3cbytes
replyed.
received: 0x62bytes
replyed.
received: 0x62bytes
replyed.
received: 0x62bytes
replyed.
received: 0x62bytes
replyed.
received: 0x62bytes
replyed.
received: 0x62bytes
replyed.
received: 0x62bytes
replyed.
received: 0x62bytes
replyed.
kzload (kozos boot loader) started.
kzload> ~
[EOT]
```

### PC側

```bash
$ ping 192.168.11.11
PING 192.168.11.11 (192.168.11.11): 56 data bytes
64 bytes from 192.168.11.11: icmp_seq=0 ttl=64 time=18.333 ms
64 bytes from 192.168.11.11: icmp_seq=1 ttl=64 time=8.401 ms
64 bytes from 192.168.11.11: icmp_seq=2 ttl=64 time=8.303 ms
64 bytes from 192.168.11.11: icmp_seq=3 ttl=64 time=8.345 ms
64 bytes from 192.168.11.11: icmp_seq=4 ttl=64 time=8.386 ms
64 bytes from 192.168.11.11: icmp_seq=5 ttl=64 time=8.363 ms
64 bytes from 192.168.11.11: icmp_seq=6 ttl=64 time=8.325 ms
64 bytes from 192.168.11.11: icmp_seq=7 ttl=64 time=8.313 ms
^C
--- 192.168.11.11 ping statistics ---
8 packets transmitted, 8 packets received, 0.0% packet loss
round-trip min/avg/max/stddev = 8.303/9.596/18.333/3.302 ms
```
