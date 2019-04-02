#ifndef _INTERRUPT_H_INCLUDED_
#define _INTERRUPT_H_INCLUDED_

#define VECTOR_NUM 64
#define VECTOR_ADDR ((void *)0xffbf20)
typedef void (*interrupt_handler_t)(int vec);

#define VECTORS ((interrupt_handler_t *)VECTOR_ADDR)

int interrupt_init();
int interrupt_sethandler(int vec, interrupt_handler_t handler);
void interrupt(int vec);

#endif
