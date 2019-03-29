#include "defines.h"


extern void start(void);          /* スタート・アップ     */
extern void intr_softerr(void);   /* ソフトウエア・エラー */
extern void intr_syscall(void);   /* システムコール       */
extern void intr_serintr(void);   /* シリアル割込み       */

/*
 * 割り込みベクタの設定
 * リンカ・スクリプの定義により、先頭番地に配置される
*/

void (*vectors[]) (void) = {
  start, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  intr_syscall, intr_softerr, intr_softerr, intr_softerr,
  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  intr_serintr, intr_serintr, intr_serintr, intr_serintr,   /* SCI0 */
  intr_serintr, intr_serintr, intr_serintr, intr_serintr,   /* SCI0 */
  intr_serintr, intr_serintr, intr_serintr, intr_serintr,   /* SCI0 */
};
