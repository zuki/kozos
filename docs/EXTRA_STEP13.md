# 移植編実装その13: elf形式から直接ロードするよう変更

## bootloadの修正

```bash
$ vi elf.[hc], xmodem.[hc], main.c
$ make clean && make
$ make write
```

## 実行

### マイコン側

```bash
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx kozos   <= motファイルではなくELFファイルを指定
Sending kozos, 194 blocks: Give your local XMODEM receive command now.
Bytes Sent:  24960   BPS:2346

Transfer complete
                 Elf loading succeeded.
kzload> run
starting from entry point: 400100
kozos boot succeed!
command> MAC: 0002cb045d12

unknown.
command> ping
ping start.
command> ICMP received: c0a80b07 00 00 08ec
ICMP received: c0a80b07 00 00 08eb
ICMP received: c0a80b07 00 00 08ea

unknown.
command> ICMP received: c0a80b07 08 00 5527
ICMP received: c0a80b07 08 00 40ef
ICMP received: c0a80b07 08 00 321c

unknown.
command> echo aaa
 aaa
command> ~
[EOT]
```

### PC側

```bash
$ ping 192.168.11.11
PING 192.168.11.11 (192.168.11.11): 56 data bytes
64 bytes from 192.168.11.11: icmp_seq=0 ttl=64 time=42.073 ms
64 bytes from 192.168.11.11: icmp_seq=1 ttl=64 time=42.136 ms
64 bytes from 192.168.11.11: icmp_seq=2 ttl=64 time=42.068 ms
^C
--- 192.168.11.11 ping statistics ---
3 packets transmitted, 3 packets received, 0.0% packet loss
round-trip min/avg/max/stddev = 42.068/42.092/42.136/0.031 ms
[dspace@mini] ~ $ telnet 192.168.11.11 80
Trying 192.168.11.11...
Connected to 192.168.11.11.
Escape character is '^]'.
GET / HTTP/1.2
HTTP/1.0 200 OK
Server: KOZOS-httpd/1.0
Content-Type: text/html
Content-Length: 373

<html>
<head>
<title>This is KOZOS!</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
</head>
<body>
<center>
<h1>HTTP server on KOZOS (14   )</h1>
<p><a href="about.html">KOZOSとは？</a></p>
<p><a href="kozos.html">KOZOSの現状</a></p>
<p><a href="makeos.html">組込みＯＳを作ってみませんか？</a></p>
</center>
</body>
</html>
Connection closed by foreign host.
```
