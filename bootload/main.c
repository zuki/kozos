#include "defines.h"
#include "interrupt.h"
#include "serial.h"
#include "xmodem.h"
#include "srec.h"
#include "elf.h"
#include "dram.h"
#include "lib.h"

static int init(void)
{
  /* 以下はリンカ・スクリプトで定義してあるシンボル */
  extern int erodata, data_start, edata, bss_start, ebss;

  /*
   * データ領域とBSS領域を初期化する。この処理以降でないと
   * グローバル変数が初期化されない
   */
  memcpy(&data_start, &erodata, (long)&edata - (long)&data_start);
  memset(&bss_start, 0, (long)&ebss - (long)&bss_start);

  /* ソフトウエア・割り込みベクタの初期化 */
  softvec_init();

  /* シリアルの初期化 */
  serial_init(SERIAL_DEFAULT_DEVICE);

  /* DRAMの初期化 */
  dram_init();

  return 0;
}

/* メモリの16進ダンプ出力 */
static int dump(char *buf, long size)
{
  long i;

  if (size < 0) {
    puts("no data.\n");
    return -1;
  }

  for (i = 0; i < size; i++) {
    putxval(buf[i], 2);
    if ((i & 0xf) == 15) {
      puts("\n");
    } else {
      if ((i & 0xf) == 7) puts(" ");
      puts(" ");
    }
  }
  puts("\n");

  return 0;
}

static void wait()
{
  volatile long i;

  for (i = 0; i < 300000; i++)
    ;
}

int main(void)
{
  static char buf[16];
  static long size = -1;
  char *entry_point;
  void (*f)(void);

  INTR_DISABLE;   /* 割込みを無効にする */

  init();

  puts("kzload (kozos boot loader) started.\n");

  while(1) {
    puts("kzload> "); /* プロンプト表示 */
    gets(buf);        /* シリアルからのコマンド受信 */

    if (!strcmp(buf, "load")) { /* elfファイルを直接ロード */
      size = load_from_xmodem();
      wait(); /* 転送アプリが終了し端末アプリに制御が戻るまで待ち合わせる */
      if (size <= 0) {
        puts("Elf loading error.\n");
      } else {
        puts("Elf loading succeeded.\n");
      }
    } else if (!strcmp(buf, "dump")) {  /* メモリの16進ダンプ出力 */
      puts("starting from entry point: ");
      putxval((unsigned long)elf_startaddr(), 0);
      puts("\n");
      dump((char *)elf_startaddr(), size);
    } else if (!strcmp(buf, "run")) {   /* ELF形式ファイルの実行 */
      entry_point = elf_startaddr();  /* メモリ上に展開（ロード） */
      if (!entry_point) {
        puts("run error!\n");
      } else {
        puts("starting from entry point: ");
        putxval((unsigned long)entry_point, 0);
        puts("\n");
        f = (void (*)(void))entry_point;
        f();  /* ロードしたプログラムに処理を渡す */
        /* ここには返ってこない */
      }
    } else if (!strcmp(buf, "ramchk")) {
      dram_check();
    } else if (!strcmp(buf, "ramchk2")) {
      dram_check2();
    } else if (!strcmp(buf, "ramclr")) {
      dram_clear();
    } else {
      puts("unknown.\n");
    }
  }

  return 0;
}
