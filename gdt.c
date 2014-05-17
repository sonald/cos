#include "gdt.h"
#include "common.h"

// Lets us access our ASM functions from our C code.
extern void gdt_flush(u32);
extern void idt_flush(u32);

static void setup_gdt_entry(int idx, u32 base, u32 limit, u16 mode);
static void setup_idt_entry(int idx, u32 base, u16 selector, u8 flags);

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

void init_gdt()
{
    gdt_ptr.limit = sizeof(gdt_entries) - 1;
    gdt_ptr.base = (u32)&gdt_entries;

    setup_gdt_entry(0, 0, 0, 0);
    setup_gdt_entry(1, 0, 0xffffffff, GDT_CODE_PL0);
    setup_gdt_entry(2, 0, 0xffffffff, GDT_DATA_PL0);
    setup_gdt_entry(3, 0, 0xffffffff, GDT_CODE_PL3);
    setup_gdt_entry(4, 0, 0xffffffff, GDT_DATA_PL3);

    gdt_flush((u32)&gdt_ptr);
}

// These extern directives let us access the addresses of our ASM ISR handlers.
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

#define SETUP_GATE(i) \
    setup_idt_entry(i, (u32)isr##i, 0x08, GDTE_PRESENT(1) | GDTE_DPL(0) | IDTE_INT_GATE)

void init_idt()
{
    idt_ptr.limit = sizeof(idt_entries) - 1;
    idt_ptr.base = (u32)&idt_entries;

    memset(&idt_entries, 0, sizeof(idt_entries));
    SETUP_GATE(0);
    SETUP_GATE(1);
    SETUP_GATE(2);
    SETUP_GATE(3);
    SETUP_GATE(4);
    SETUP_GATE(5);
    SETUP_GATE(6);
    SETUP_GATE(7);
    SETUP_GATE(8);
    SETUP_GATE(9);
    SETUP_GATE(10);
    SETUP_GATE(11);
    SETUP_GATE(12);
    SETUP_GATE(13);
    SETUP_GATE(14);
    SETUP_GATE(15);
    SETUP_GATE(16);
    SETUP_GATE(17);
    SETUP_GATE(18);
    SETUP_GATE(19);
    SETUP_GATE(20);
    SETUP_GATE(21);
    SETUP_GATE(22);
    SETUP_GATE(23);
    SETUP_GATE(24);
    SETUP_GATE(26);
    SETUP_GATE(27);
    SETUP_GATE(28);
    SETUP_GATE(29);
    SETUP_GATE(30);
    SETUP_GATE(31);

    idt_flush((u32)&idt_ptr);
}

#undef SETUP_GATE

void setup_gdt_entry(int idx, u32 base, u32 limit, u16 mode)
{
    gdt_entry_t* p = &gdt_entries[idx];
    p->limit_low = (limit & 0xffff);
    p->base_low = (base & 0xffff);
    p->base_middle = (base >>16) & 0xff;
    p->access = mode & 0xff;
    p->granularity = ((mode >> 8) & 0xf0) | ((limit >> 16) & 0x0f);
    p->base_high = (base >> 24) & 0xff;
}

void setup_idt_entry(int idx, u32 base, u16 selector, u8 flags)
{
    idt_entry_t *p = &idt_entries[idx];
    p->base_lo = (base & 0xffff);
    p->always0 = 0;
    p->flags = flags;
    p->sel = selector;
    p->base_hi = (base >> 16) & 0xffff;
}
