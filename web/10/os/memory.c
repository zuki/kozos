#include "kozos.h"
#include "memory.h"

#include "lib.h"

#define MEMORY_AREA1_SIZE 32
#define MEMORY_AREA1_NUM  16
#define MEMORY_AREA2_SIZE 48
#define MEMORY_AREA2_NUM  8
#define MEMORY_AREA3_SIZE 64
#define MEMORY_AREA3_NUM  4

static char memory_area_1[MEMORY_AREA1_SIZE * MEMORY_AREA1_NUM];
static char memory_area_2[MEMORY_AREA2_SIZE * MEMORY_AREA2_NUM];
static char memory_area_3[MEMORY_AREA3_SIZE * MEMORY_AREA3_NUM];

typedef struct _kzmem {
  struct _kzmem *next;
  int size;
  char dummy[2];
} kzmem;

typedef struct _kzmem_pool {
  char *area;
  int size;
  int num;
  kzmem *free;
  char dummy[4];
} kzmem_pool;

static kzmem_pool pool[] = {
  { memory_area_1, MEMORY_AREA1_SIZE, MEMORY_AREA1_NUM, NULL },
  { memory_area_2, MEMORY_AREA2_SIZE, MEMORY_AREA2_NUM, NULL },
  { memory_area_3, MEMORY_AREA3_SIZE, MEMORY_AREA3_NUM, NULL },
};

#define MEMORY_AREA_NUM (sizeof(pool) / sizeof(*pool))

static int kzmem_init_pool(kzmem_pool *p)
{
  int i;
  kzmem *mp;
  kzmem **mpp;

  mp = (kzmem *)p->area;
  mpp = &p->free;
  for (i = 0; i < p->num; i++) {
    *mpp = mp;
    memset(mp, 0, sizeof(*mp));
    mp->size = p->size;
    mpp = &(mp->next);
    mp = (kzmem *)((char *)mp + p->size);
  }

  return 0;
}

int kzmem_init()
{
  int i;
  for (i = 0; i < MEMORY_AREA_NUM; i++) {
    kzmem_init_pool(&pool[i]);
  }
  return 0;
}

void *kzmem_alloc(int size)
{
  int i;
  kzmem *mp;
  kzmem_pool *p;

  for (i = 0; i < MEMORY_AREA_NUM; i++) {
    p = &pool[i];
    if (size <= p->size - sizeof(kzmem)) {
      if (p->free == NULL) {
	kz_sysdown();
      }
      mp = p->free;
      p->free = p->free->next;
      mp->next = NULL;
      return (char *)mp + sizeof(kzmem);
    }
  }

  kz_sysdown();
  return NULL;
}

void kzmem_free(void *mem)
{
  int i;
  kzmem *mp;
  kzmem_pool *p;

  mp = (kzmem *)((char *)mem - sizeof(kzmem));

  for (i = 0; i < MEMORY_AREA_NUM; i++) {
    p = &pool[i];
    if (mp->size == p->size) {
      mp->next = p->free;
      p->free = mp;
      return;
    }
  }

  kz_sysdown();
}
