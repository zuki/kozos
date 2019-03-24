#include "defines.h"

extern void start(void);    /* in startup.s */

/*
 * 割り込みベクタの設定
 * リンカ・スクリプの定義により、先頭番地に配置される
*/

void (*vectors[]) (void) = {
  start, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};
