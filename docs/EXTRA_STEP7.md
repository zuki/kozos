# 移植編実装その7: [(H8移植編その２第５回)IP通信をごっそり書き直した](http://kozos.jp/kozos/h8_2_05.html)

## osの修正

```bash
$ rm etherdrv.[ch], ether.[ch]
$ vi rtl8019.[ch], arp.[ch], ethernet.[ch]
$ vi ip.h, icmp.[ch], netdrv.[ch]
$ vi ip.c, command.c, kozos.[ch], main.c, memory.c
$ make clear && make && make image
```

## 実行

### PCでの操作

```bash
$ ping 192.168.11.11
PING 192.168.11.11 (192.168.11.11): 56 data bytes
64 bytes from 192.168.11.11: icmp_seq=0 ttl=64 time=35.276 ms
64 bytes from 192.168.11.11: icmp_seq=1 ttl=64 time=35.339 ms
64 bytes from 192.168.11.11: icmp_seq=2 ttl=64 time=35.238 ms
64 bytes from 192.168.11.11: icmp_seq=3 ttl=64 time=35.312 ms
^C
--- 192.168.11.11 ping statistics ---
4 packets transmitted, 4 packets received, 0.0% packet loss
round-trip min/avg/max/stddev = 35.238/35.291/35.339/0.038 ms
```

## マイコン側

```bash
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx kozos.mot
Sending kozos.mot, 224 blocks: Give your local XMODEM receive command now.
Bytes Sent:  28800   BPS:2781

Transfer complete
                 XMODEM receive succeeded.
kzload> run
starting from entry point: 400100
kozos boot succeed!
command> MAC: 0002cb045d12
MAC: 0002cb045d12

unknown.
command> ping
ping start.
command> ICMP received: c0a80b07 00 00 08ec
ICMP received: c0a80b07 00 00 08eb
ICMP received: c0a80b07 00 00 08ea

unknown.
command> ICMP received: c0a80b07 08 00 c573    # <= PCからのping受信
ICMP received: c0a80b07 08 00 b2a4
ICMP received: c0a80b07 08 00 a9ed
ICMP received: c0a80b07 08 00 9791
```
