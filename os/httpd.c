#include "defines.h"
#include "kozos.h"
#include "netdrv.h"
#include "tcp.h"
#include "lib.h"

static char header[] =
"HTTP/1.0 200 OK\r\n"
/*"Date: Sat, 23 Oct 2010 12:00:00 GMT\r\n"*/
"Server: KOZOS-httpd/1.0\r\n"
"Content-Type: text/html\r\n"
"Content-Length: #####\r\n"
"\r\n";

static char top_document[] =
"<html>\n"
"<head>\n"
"<title>This is KOZOS!</title>\n"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
"</head>\n"
"<body>\n"
"<center>\n"
"<h1>HTTP server on KOZOS (#####)</h1>\n"
"<p><a href=\"about.html\">KOZOSとは？</a></p>\n"
"<p><a href=\"kozos.html\">KOZOSの現状</a></p>\n"
"<p><a href=\"makeos.html\">組込みＯＳを作ってみませんか？</a></p>\n"
"</center>\n"
"</body>\n"
"</html>\n";

static char about_document[] =
"<html>\n"
"<head>\n"
"<title>KOZOSとは？</title>\n"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
"</head>\n"
"<body>\n"
"<center><h1>KOZOSとは？(#####)</h1></center>\n"
"<p>KOZOSは坂井弘亮氏が作成している，学習向けの組込みOSです．主に秋月電子のH8/3069Fマイコンボードで動作し，組込みOSの自作・動作などの学習ができます．</p>\n"
"<p>以下の点で，個人での学習用に向いています．\n"
"<ul>\n"
"<li>従来の組込みOS教材に比べ，ソースコード量が1800行程度と少ないため初学者でも読解可能．</li>\n"
"<li>動作するマイコンボードが4000円程度と安価なので，個人でも購入可能．また実習などで利用する場合にも，生徒ひとりにひとつ教材を割り当て可能な価格．</li>\n"
"<li>組み立て済みボードのため半田付けなどが不要で，買ったその日からの学習が可能．</li>\n"
"<li>シリアル経由でフラッシュROM書き換え可能なため，ICEやROMライタなどが不要．</li>\n"
"<li>CPU(H8)やボードが日本製のため日本語の資料が豊富．</li>\n"
"<li>ブートローダーを含めたフルスクラッチOSであるため本当の最初からの学習が可能．</li>\n"
"<li>開発環境はGNU環境であるため特定のCPUのコンパイラに依存しないつぶしの効く学習が可能．</li>\n"
"<li>書籍「12ステップで作る 組込みＯＳ自作入門」に詳細な解説あり．</li>\n"
"<li>オープンソース・ライセンスのため入手や改造が自由．</li>\n"
"</ul>\n"
"<p>組込みOSを利用して制御実習など行うというよりも，組込みOSの内部構造や動作原理そのものを理解する，自分で改造を加えて理解を深めるといった学習に向いています．「実用的に使うためのOS」でなく「内部構造を理解し，改造したり機能追加したりしていじって遊ぶためのOS」とご理解ください．</p>\n"
"<hr>\n"
"<ul>\n"
"<li><a href=\"index.html\">トップページに戻る</a>\n"
"</ul>\n"
"</body>\n"
"</html>\n";

static char kozos_document[] =
"<html>\n"
"<head>\n"
"<title>KOZOSの現状</title>\n"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
"</head>\n"
"<body>\n"
"<center><h1>KOZOSの現状(#####)</h1></center>\n"
"<p>以下がすでに実現されています．</p>\n"
"<ul>\n"
"<li>ＯＳとしての最低限の機能を持つミニマムなカーネル</li>\n"
"<li>独自のブートローダー</li>\n"
"<li>マルチタスク動作，メモリ管理，I/O管理(割込み管理)，タスク間通信</li>\n"
"<li>H8，SH2，PowerPCなどでの動作</li>\n"
"<li>シリアル経由でのコマンド応答</li>\n"
"<li>DRAM対応</li>\n"
"<li>タイマサービス</li>\n"
"<li>ethernetとTCP/IPによる通信機能</li>\n"
"<li>簡易webサーバ</li>\n"
"<li>シミュレータ上での動作</li>\n"
"<li>書籍「12ステップで作る 組込みＯＳ自作入門」の出版</li>\n"
"</ul>\n"
"<p>以下は今後進めていく予定の作業一覧です．</p>\n"
"<ul>\n"
"<li>デバッガ対応</li>\n"
"<li>ファイル転送機能</li>\n"
"<li>アプリケーションの動的実行</li>\n"
"<li>セマフォ実装</li>\n"
"<li>ROM化</li>\n"
"<li>リアルタイム化</li>\n"
"<li>カーネル内部にデバッグ機能を実装</li>\n"
"<li>SH2への本対応</li>\n"
"<li>「12ステップで作る 組込みＯＳ自作入門」の続編を出版</li>\n"
"</ul>\n"
"<hr>\n"
"<ul>\n"
"<li><a href=\"index.html\">トップページに戻る</a>\n"
"</ul>\n"
"</body>\n"
"</html>\n";

static char makeos_document[] =
"<html>\n"
"<head>\n"
"<title>組込みOSを作ってみませんか？</title>\n"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
"</head>\n"
"<body>\n"
"<center><h1>組込みOSを作ってみませんか？(#####)</h1></center>\n"
"<p>「組込み」「組込みシステム」「エンベデッド」という言葉が一般でもよく聞かれるようになってきました．組込み分野に興味のある人の潜在数はなかなか多いと思うのですが，それと同時に「とっつきにくい」「よくわからない」という話もちらほらと聞くことがあります．なぜ「とっつきにくい」のでしょうか？</p>\n"
"<p>組込みシステムの勉強をしたいと思ったとき，いちばん手っ取り早いのは適当なボードコンピュータを入手して，LinuxなりITRONなりといったOSをインストールして，自分でいじってみることです．さらにそのOSを読んで内部動作を理解したり，OSに手を入れて改造したりしていければバッチリでしょう．</p>\n"
"<p>しかしLinuxのソースコード量をOS初心者が独力で読み解いていくのは，相当な根気が必要でしょう．動作の中核だけ読むとしても，それがどこなのかを判別するだけで一苦労です．それはITRONの実装であるTOPPERSやHOSについても同様でしょう．これは多CPU対応，多機能化のために階層化して抽象度を高くしていった結果なので実用OSとしては正しい姿だと思うのですが，そのために関数呼び出しやマクロ定義などの階層が深く読みにくくなっていることは事実で，OS初心者が独力でパッと読み解けるようなものではなくなっています．</p>\n"
"<p>ではもっと手軽なOSをベースにして学習すればいいのでしょうか？世の中にはオープンソースで手軽な組込みOSは多数あります．しかしここでもうひとつ問題になるのは，そのOSが動作するボードです．これらの独自OSの多くはCPUの評価ボードなどでの動作を前提にしています．評価ボードはOS移植という点だけで見ると最適な題材なのですが，個人利用という点で見ると，致命的な欠陥があります．それはなんといっても「お高い」のです．数万円〜十数万円するのがザラです．研究目的や業務のために研究室や会社で購入するならば，どうということはないのでしょう．しかしOS勉強のために個人で購入するには，ちょっと気軽には買えない金額です．</p>\n"
"<p>組込みOSの書籍も様々ありますが，初心者が個人レベルで学習できるということを真剣に考えて書かれたものは本当に少なく思います．対象とするOSが有料で数万円もしたり，有償の開発環境を利用していたり，自作した特殊なボードを対象にしているものもあります．これでは現実として，個人レベルで試せません．ほんとうに混沌とした世界です．このあたりが「組込みは難しい」と思われている理由のように思います．またこれらの書籍はOSを「使う」ためのものであって，OSを「作る」ための説明がなされているものはほとんどありません．</p>\n"
"<p>中学生や高校生でもお小遣いの範囲で買えるような金額のボードで動作して，初心者でも十分に読解できて気軽に改造できるような単純なソースコード構成で，ビルドに特殊なスクリプト実行などを必要とせず，開発環境が無償で入手できて，特定の環境にしばられないためつぶしが利く学習ができて，オープンソースであること．個人の学習用には，このような条件が必須です．しかしそのようなOS自作の学習用のサンプルになる組込みOSが無いのです．ていうか，私自身がそういうものがすごく欲しかったのです．\n"
"<p>実用OSとして設計されたOSでは，これらの条件を全て満たすことは不可能です．なぜならば，設計思想が初めから異なっているからです．初めから学習目的として設計された組込みOSが必要なのです．</p>\n"
"<p>KOZOSはそのような考えで作られています．PCさえあれば，あとは5000円くらいでひととおりの設備はそろって，初心者でも読みきれるコード量．誰でも簡単に改造できて，パッといろいろ試すことができる．そんなポリシーで作られています．もちろんオープンソース，フリーソフトです．</p>\n"
"<p>H8用のKOZOSではブートローダーが１０００行程度，OSの本体が２０００行程度です．これならば，自分でも読みきれるような気がしてきませんか？ ちょっと頑張れば自分にも作れるのではないか，という気になるのではないでしょうか．しかもボードは4000円弱，完成品ボードなので半田付けいらず，シリアル経由でフラッシュROMの書き換え可能なのでROMライタ不要だしROMの内容を壊しても安心，通販で誰でも購入可能です．開発環境はFreeBSD，GNU/Linux，CygwinのいずれかでGNU開発環境を想定しているので応用範囲も広く，後々につぶしが利く学習ができます．国産ボード／国産CPUなので日本語資料も充実．ほんとに誰でも試せます．</p>\n"
"<p>この機会に組込みOSを勉強してみるのはどうでしょうか．一歩進んで，OSを自作してみるのはどうでしょうか？OS自作は勉強になるし，とても面白いですよ！</p>\n"
"<hr>\n"
"<ul>\n"
"<li><a href=\"index.html\">トップページに戻る</a>\n"
"</ul>\n"
"</body>\n"
"</html>\n";

static char unknown_document[] =
"<html>\n"
"<head>\n"
"<title>Unknown Request</title>\n"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
"</head>\n"
"<body>\n"
"<center>\n"
"<h1>Unknown Request (#####)</h1>\n"
"<p><a href=\"index.html\">トップページ</a>\n"
"</center>\n"
"</body>\n"
"</html>\n";

static struct documents {
  char *counterp;
  char *filename;
  char *document;
} documents[] = {
  { NULL, "/index.html",  top_document },
  { NULL, "/about.html",  about_document },
  { NULL, "/kozos.html",  kozos_document },
  { NULL, "/makeos.html", makeos_document },
  { NULL, NULL, unknown_document }
};

static void send_accept(void)
{
  struct netbuf *buf;
  buf = kz_kmalloc(sizeof(*buf));
  buf->cmd = TCP_CMD_ACCEPT;
  buf->option.tcp.accept.id = MSGBOX_ID_HTTPD;
  buf->option.tcp.accept.port = 80;
  kz_send(MSGBOX_ID_TCPPROC, 0, (char *)buf);
}

static void send_close(int number)
{
  struct netbuf *buf;

  buf = kz_kmalloc(sizeof(*buf));
  buf->cmd = TCP_CMD_CLOSE;
  buf->option.tcp.close.number = number;
  kz_send(MSGBOX_ID_TCPPROC, 0, (char *)buf);
}

static void send_write(int number, int size, char *data)
{
  struct netbuf *buf;

  buf = kz_kmalloc(DEFAULT_NETBUF_SIZE);
  memset(buf, 0, DEFAULT_NETBUF_SIZE);
  buf->cmd  = TCP_CMD_SEND;
  buf->top  = buf->data;
  buf->size = size;
  buf->option.tcp.send.number = number;
  memcpy(buf->top, data, size);
  kz_send(MSGBOX_ID_TCPPROC, 0, (char *)buf);
}

static void send_string(int number, char *str)
{
  int len, size;
  len = strlen(str);
  while (len > 0) {
    size = (len > 512) ? 512 : len;
    send_write(number, size, str);
    len -= size;
    str += size;
  }
}

static char *val2str(int value)
{
  static char str[11];
  int i;
  char *top;

  memset(str, '0', sizeof(str));
  str[10] = '\0';
  top = &str[9];

  for (; value > 0; value--) {
    str[9]++;
    for (i = 10 - 1; i >= 0; i--) {
      if ((str[i] > '9') && (i > 0)) {
        str[i] = '0';
        str[i - 1]++;
      }
      if ((&str[i] < top) && (str[i] > '0'))
        top = &str[i];
    }
  }

  return top;
}

static int parse(int number, char *str)
{
  static int count = 0;
  static char *length_p = NULL;
  char *filename, *p;
  struct documents *docs;

  if (strncmp(str, "GET", 3))
    return 0;

  for (p = str + 3; *p == ' '; p++)
    ;

  filename = p;
  p = strchr(p, ' ');
  *p = '\0';

  if (!strcmp(filename, "/"))
    filename = "/index.html";

  for (docs = documents; docs->filename; docs++)
    if (!strcmp(filename, docs->filename))
      break;

  if (!docs->counterp) docs->counterp = strchr(docs->document, '#');
  memset(docs->counterp, ' ', 5);
  p = val2str(count);
  memcpy(docs->counterp, p, strlen(p));
  count++;

  if (!length_p) length_p = strchr(header, '#');
  memset(length_p, ' ', 5);
  p = val2str(strlen(docs->document));
  memcpy(length_p, p, strlen(p));

  send_string(number, header);
  send_string(number, docs->document);
  return 1;
}

int httpd_main(int argc, char *argv[])
{
  char *p = NULL, *r;
  char *buffer;
  int number = 0, ret;
  struct netbuf *buf;

  send_accept();

  buffer = kz_kmalloc(DEFAULT_NETBUF_SIZE);

  while (1) {
    kz_recv(MSGBOX_ID_HTTPD, NULL, (char **)&buf);

    switch (buf->cmd) {
      case TCP_CMD_ESTAB:
        number = buf->option.tcp.establish.number;
        p = buffer;
        break;

      case TCP_CMD_CLOSE:
        number = 0;
        send_accept();
        break;

      case TCP_CMD_RECV:
        memcpy(p, buf->top, buf->size);
        p += buf->size;
        *p = '\0';

        r = strchr(buffer, '\n');
        if (r) {
          *r = '\0';
          r++;
          ret = parse(number, buffer);
          memmove(buffer, r, p - r + 1);
          p -= (r - buffer);
          if (ret) send_close(number);
        }
        break;
    }
    kz_kmfree(buf);
  }
  return 0;
}
