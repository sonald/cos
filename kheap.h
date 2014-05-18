#ifndef _KHEAP_H
#define _KHEAP_H 

#include "common.h"

u32 kmalloc_a(u32 sz);
u32 kmalloc_p(u32 sz, u32 *phys);
u32 kmalloc_ap(u32 sz, u32 *phys);
u32 kmalloc(u32 sz);

#endif
