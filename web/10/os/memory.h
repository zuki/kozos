#ifndef _KOZOS_MEMORY_H_INCLUDED_
#define _KOZOS_MEMORY_H_INCLUDED_

int kzmem_init();
void *kzmem_alloc(int size);
void kzmem_free(void *mem);

#endif
