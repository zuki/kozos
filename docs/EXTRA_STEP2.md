# 移植編実装その2: シリアル接続の速度を上げる

## bootload, osの修正

```bash
$ vi bootload/serial.c
$ git diff bootload/serial.c
diff --git a/bootload/serial.c b/bootload/serial.c
index 0ea2a0a..5a59e27 100644
--- a/bootload/serial.c
+++ b/bootload/serial.c
@@ -64,7 +64,7 @@ int serial_init(int index)

   sci->scr = 0;
   sci->smr = 0;
-  sci->brr = 64;      /* 20MHzのクロックから9600bpsを生成 */
+  sci->brr = 15;      /* 20MHzのクロックから38400bpsを生成 */
   sci->scr = H8_3069F_SCI_SCR_RE | H8_3069F_SCI_SCR_TE;   /* 送受信可能 */
   sci->ssr = 0;

$ make
$ vi os/serial.c       # bootload/serial.cと同じ修正
$ make
$ make image
```

## 実行

手元の環境では`38400bps`まで正常に通信できた。再試するたびに速度が上がっているのはキャッシュのせいか？

```bash
$ cu -l /dev/cu.usbserial
Connected
kzload (kozos boot loader) started.
kzload> ~
unknown.
kzload> load
~CLocal command? lsx kozos.mot
Sending kozos.mot, 114 blocks: Give your local XMODEM receive command now.
Bytes Sent:  14720   BPS:752     # 9600bps
Bytes Sent:  14720   BPS:1293    # 19200bps
Bytes Sent:  14720   BPS:1757    # 38400bps(1回目)
Bytes Sent:  14720   BPS:2268    # 38400bps(2回目)
Bytes Sent:  14720   BPS:3606    # 38400bps(2回目)

Transfer complete
                 XMODEM receive succeeded.
kzload> run
starting from entry point: ffc020
kozos boot succeed!
command> echo aaa
 aaa
command> ~
[EOT]
$ echo "alias cu='cu -s 38400'" >> ~/.bashrc
$ source ~/.bashrc
```
