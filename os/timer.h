#ifndef _TIMER_H_INCLUDED_
#define _TIMER_H_INCLUDED_

#define TIMER_START_FLAG_CYCLE (1<<0)

int timer_start(int index, int msec, int flags); /* タイマ開始 */
int timer_is_expired(int index);         /* タイマ満了したか？ */
int timer_expire(int index);             /* タイマ満了処理     */
int timer_cancel(int index);             /* タイマキャンセル   */
int timer_is_running(int index);         /* タイマ動作中か？   */
int timer_gettime(int index);            /* タイマの現在値     */

#endif
