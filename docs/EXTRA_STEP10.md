# 移植編実装その10: [(H8移植編その２第10回)TCPの再送処理の実装準備](http://kozos.jp/kozos/h8_2_10.html)

topページは取れるが、次のリンクが転送されない。topページ送信後も定期的にLEDがついているので、topページの送受信が切れていないのではないか。

## 接続時のwiresharkキャプチャ画面

![wireshark capture screen of extra-step 10](wireshark-step10.png)

## 移植編実装その9の時点でのキャプチャ画面

![wireshark capture screen of extra-step 9](wireshark-step9.png)

本ステップにおける修正文のうち、`tcp_recv_close()`該当部分を一部修正することで問題がなくなった。

```diff
$ diff -uw ../web/10/os/tcp.c tcp.c
--- ../web/10/os/tcp.c	2019-04-08 16:49:19.000000000 +0900
+++ tcp.c	2019-04-09 09:53:11.000000000 +0900
@@ -560,34 +397,26 @@
       new_status = TCP_CONNECTION_STATUS_SYNRECV;
     }

-    /* SYNSENTならばACKを返す(たぶんSYN+ACKが来ている) */
+    /* SYNSENTならばACKを返す（たぶんSYN+ACKが来ていている） */
     if (con->status == TCP_CONNECTION_STATUS_SYNSENT) {
       con->ack_number = tcphdr->seq_number + 1;
       tcp_makesendpkt(con, TCP_HEADER_FLAG_ACK, 1460, 0, 0, 0, NULL);
     }
   }

-  if (tcphdr->flags & TCP_HEADER_FLAG_FIN) {
-    /* FINWAIT2なら，ACKを返してCLOSEDに遷移 */
+  if ((tcphdr->flags & TCP_HEADER_FLAG_FINACK) == TCP_HEADER_FLAG_FINACK) {
+      /* FINWAIT2なら、ACKを返してCLOSEDに遷移 */
     if (con->status == TCP_CONNECTION_STATUS_FINWAIT2) {
       con->ack_number = tcphdr->seq_number + 1;
       tcp_makesendpkt(con, TCP_HEADER_FLAG_ACK, 1460, 0, 0, 0, NULL);
       new_status = TCP_CONNECTION_STATUS_CLOSED;
       closed++;
-    }
-
-    /* ESTABなら，ACK, FIN+ACK を返してLASTACKに遷移 */
-    if (con->status == TCP_CONNECTION_STATUS_ESTAB) {
+      } else {
+      /* ESTABなら、Ack, FIN+ACK を返してLASTACKに遷移 */
       con->ack_number = tcphdr->seq_number + 1;
       tcp_makesendpkt(con, TCP_HEADER_FLAG_ACK, 1460, 0, 0, 0, NULL);
-      /* new_status = TCP_CONNECTION_STATUS_CLOSEWAIT; */
-      tcp_makesendpkt(con,
-#if 0
-		      TCP_HEADER_FLAG_FIN,
-#else
-		      TCP_HEADER_FLAG_FINACK,
-#endif
-		      1460, 0, 0, 0, NULL);
+        new_status = TCP_CONNECTION_STATUS_CLOSEWAIT;
+        tcp_makesendpkt(con, TCP_HEADER_FLAG_FINACK, 1460, 0, 0, 0, NULL);
       new_status = TCP_CONNECTION_STATUS_LASTACK;
     }
   }
```
