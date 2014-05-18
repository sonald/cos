#include "boot.h"
#include "common.h"
#include "gdt.h"
#include "timer.h"
#include "mm.h"

extern u32 mem_end_page;
extern u32 _end;

int main(struct multiboot_info *mb)
{
    init_gdt();
    init_idt();
    init_timer();

    clear();
    const char* msg = "Welcome to COS....";
    kputs(msg);
    if (mb->flags & 0x1) {
        u32 memsize = mb->low_mem + mb->high_mem;
        mem_end_page = (memsize & 0xfffff000) + 0x1000;
        kprintf("detected mem: low: %dKB, hi: %dKB\n", mb->low_mem, mb->high_mem);
    }

    init_mm();

    __asm__ __volatile__ ("sti");

    //page fault
    u32* invalid = (u32*)0xA0000000;
    kputchar((char)*invalid);

    return 0x1BADFEED;
}
