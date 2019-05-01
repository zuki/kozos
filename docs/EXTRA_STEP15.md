# 移植編実装その15: ntpクライアントの実装

## osの編集

```bash
$ vi ntp.[hc]
$ vi arp.c, command.c, ip.c, main.c, netdrv.h, defines.h, kozos.h, Makefile
$ make clean && make
```

## 実行

```
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> load
~CLocal command? lsx kozos
Sending kozos, 212 blocks: Give your local XMODEM receive command now.
Bytes Sent:  27136   BPS:2348

Transfer complete
                 Elf loading succeeded.
kzload> run
starting from entry point: 400100
kozos boot succeed!
command> MAC: 0002cb045d12

unknown.
command> date
command> 2019/05/02 12:17:45 JST

unknown.
command> ~
[EOT]
```

## 学習した点

### 1. Internetへの接続

Internetに接続する場合は、ethernetフレームの宛先アドレスにデフォルトゲートウェイのアドレスを指定すれば良かった。これを実装するために、サブネットマスクとデフォルトゲートウェイアドレスをip.cに持ち、新たにオプション変数 `option.ethernet.send.router_ipaddr`を追加して、ethernetフレームに設定する宛先アドレレスを設定し、ethernetフレームを作成する際の宛先アドレスにはこちらを使用するようにした。

```diff
diff --git a/os/ip.c b/os/ip.c
index 34d72a0..50ffc62 100644
--- a/os/ip.c
+++ b/os/ip.c
@@ -21,8 +21,13 @@ struct ip_header {
   uint32 dst_addr;
 };

-#define IPADDR  0xc0a80b0b   /* 192.168.11.11 */
-static uint32 my_ipaddr = IPADDR;
+#define IPADDR          0xc0a80b0b   /* 192.168.11.11 */
+#define SUBNET_MASK     0xffffff00   /* 255.255.255.0 */
+#define DEFAULT_GATEWAY 0xc0a80b01   /* 192.168.11.1  */
+
+static uint32 my_ipaddr  = IPADDR;
+static uint32 my_subnet  = SUBNET_MASK;
+static uint32 my_gateway = DEFAULT_GATEWAY;

 #define PROTOCOL_MAXNUM IP_PROTOCOL_UDP   /* UDPまで利用可能 */

@@ -114,6 +119,7 @@ static int ip_send(struct netbuf *pkt)
   memset(pkt->option.ethernet.send.dst_macaddr, 0, MACADDR_SIZE);
   pkt->option.ethernet.send.type = ETHERNET_TYPE_IP;
   pkt->option.ethernet.send.dst_ipaddr = iphdr->dst_addr;
+  pkt->option.ethernet.send.router_ipaddr = ((iphdr->dst_addr & my_subnet) == (my_ipaddr & my_subnet)) ? iphdr->dst_addr : my_gateway;
   kz_send(MSGBOX_ID_ETHPROC, 0, (char *)pkt);

   return 1;
```

### 2. アルゴリズムは大事

H8では32ビットの割り算ができない（ライブラリが必要）ので、エポック秒から日時に変換する際に、割り算用の関数を用意したが、何も考えずに被除数から除数を引いた回数を数えるロジックで実装したところ、ntpdateコマンドを発行後、日時が表示されるまで4分以上かかった。

インターネットを介した処理の実装も初めてだったため、そもそも4分後に正常に日付が表示されることに気付くのも遅れたが、気づいた後もこの4分がどこで発生しているかが皆目検討がつかなかった。ntpdateコマンドを実行後、echoクライントを実行するとechoコマンドは待たされるが、pingやhttpdは正常に稼働することがわかり、udp関係のデッドロックの発生が想定された。しかし、ntp.cに細かくデバックプリントを入れたところ、割り算関数で時間がかかっている可能性が浮上した。

改めて割り算のアルゴリズムを調べて実装したところ一瞬で計算されるようになった。アルゴリズムは重要である。

## NTPサーバにおけるtcpdump

今回はテストということでubuntuサーバで稼働中のntpdをntpサーバとして利用した。

```
sudo tcpdump -e -XX port 123
tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
listening on eth0, link-type EN10MB (Ethernet), capture size 262144 bytes
09:52:51.875068 40:ce:24:ae:dd:ff (oui Unknown) > 52:54:06:00:21:42 (oui Unknown), ethertype IPv4 (0x0800), length 90: p1771012-ipbf1804sapodori.hokkaido.ocn.ne.jp.ntp > www.zuki-ebetsu.jp.ntp: NTPv3, Client, length 48
	0x0000:  5254 0600 2142 40ce 24ae ddff 0800 4500  RT..!B@.$.....E.
	0x0010:  004c 0005 0000 2f11 c0c6 99ce 590c db5e  .L..../.....Y..^
	0x0020:  fc9c 007b 007b 0038 18b2 1b00 0000 0000  ...{.{.8........
	0x0030:  0000 0000 0000 0000 0000 0000 0000 0000  ................
	0x0040:  0000 0000 0000 0000 0000 0000 0000 0000  ................
	0x0050:  0000 0000 0000 0000 0000                 ..........
09:52:51.875291 52:54:06:00:21:42 (oui Unknown) > 00:00:0c:9f:f0:01 (oui Cisco), ethertype IPv4 (0x0800), length 90: www.zuki-ebetsu.jp.ntp > p1771012-ipbf1804sapodori.hokkaido.ocn.ne.jp.ntp: NTPv3, Server, length 48
	0x0000:  0000 0c9f f001 5254 0600 2142 0800 45b8  ......RT..!B..E.
	0x0010:  004c fe9e 4000 4011 7074 db5e fc9c 99ce  .L..@.@.pt.^....
	0x0020:  590c 007b 007b 0038 cb1f 1c02 03eb 0000  Y..{.{.8........
	0x0030:  0232 0000 0c2b 85f3 eea4 e074 b5ab 3e98  .2...+.....t..>.
	0x0040:  9eb4 0000 0000 0000 0000 e074 bde3 e004  ...........t....
	0x0050:  81e8 e074 bde3 e010 30bd                 ...t....0.
```
