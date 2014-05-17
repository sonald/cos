#include "isr.h"

void isr_handler(registers_t regs)
{
    kprintf("regs: ds: 0x%x, edi: 0x%x, esi: 0x%x, ebp: 0x%x, esp: 0x%x\n"
            "ebx: 0x%x, edx: 0x%x, ecx: 0x%x, eax: 0x%x, isr: %d, errno: %d\n"
            "eip: 0x%x, cs: 0x%x, eflags: 0b%b, useresp: 0x%x, ss: 0x%x\n",
            regs.ds, regs.edi, regs.esi, regs.ebp, regs.esp, 
            regs.ebx, regs.edx, regs.ecx, regs.eax, regs.isrno, regs.errcode,
            regs.eip, regs.cs, regs.eflags, regs.useresp, regs.ss);
}

