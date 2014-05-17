#ifndef _ISR_H
#define _ISR_H 

#include "common.h"
#include "gdt.h"

typedef struct registers_s {
    u32 ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 isrno, errcode;
    // Pushed by the processor automatically.
    u32 eip, cs, eflags;
    u32 useresp, ss;  // exists only when PL switched
} __attribute__((packed)) registers_t;

void isr_handler(registers_t regs);
#endif
