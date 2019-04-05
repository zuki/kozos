#ifndef _INTR_H_INCLUDED_
#define _INTR_H_INCLUDED_

/* ソフトウェア・割り込みベクタの定義 */

#define SOFTVEC_TYPE_NUM      4

#define SOFTVEC_TYPE_SOFTERR  0   /* ソフトウエア・エラー */
#define SOFTVEC_TYPE_SYSCALL  1   /* システムコール       */
#define SOFTVEC_TYPE_SERINTR  2   /* シリアル割込み       */
#define SOFTVEC_TYPE_TIMINTR  3   /* タイマ割込み         */

#endif
