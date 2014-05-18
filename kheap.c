#include "kheap.h"
#include "common.h"

u32 _placement_addr;

u32 kmalloc_helper(u32 sz, int align, u32 *phys)
{
    if (align && (_placement_addr & 0xfffff000)) {
        _placement_addr = (_placement_addr & 0xfffff000) + (1<<12);
    }

    u32 ret = _placement_addr;
    if (phys) *phys = ret;
    kprintf("alloc %d; ", sz);
    _placement_addr += sz;
    return ret;
}

u32 kmalloc_a(u32 sz)
{
    return kmalloc_helper(sz, 1, NULL);
}

u32 kmalloc_p(u32 sz, u32 *phys)
{
    return kmalloc_helper(sz, 0, phys);
}

u32 kmalloc_ap(u32 sz, u32 *phys)
{
    return kmalloc_helper(sz, 1, phys);
}

u32 kmalloc(u32 sz)
{
    return kmalloc_helper(sz, 0, NULL);
}

