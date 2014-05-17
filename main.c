#include "boot.h"
#include "common.h"
#include "gdt.h"
#include "timer.h"

int main(struct multiboot_info *mb)
{
    init_gdt();
    init_idt();
    init_timer();

    const char* msg = "\nWelcome to COS....";
    /*clear();*/
    kputs(msg);
    kputs("\n \tBooting...\n\n\nchecking....\n");
    kputs("1\t2\t3\t4\t5\t6\t7\t8\t9\t0\t");
    kputs(itoa(1314, 10));
    kprintf("%s\n", "substring");
    kprintf("%d, %d, %c, %d, %s, %c\n", 12, 34, '|', 56, "incredible", ')');
    kprintf("va args %%d: %d, %%c: %c, %%s: %s, %d\n", 1314, '#', "substring", 798);

    __asm__ __volatile__ ("int $0x03");
    __asm__ __volatile__ ("sti");

    return 0x1BADFEED;
}
