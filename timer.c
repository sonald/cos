#include "timer.h"
#include "isr.h"

static const u32 FREQ = 1193180;
static u32 tick = 0;

static void timer_interrupt(registers_t regs)
{
    u16 cur = get_cursor();
    set_cursor(CURSOR(68, 24));
    kprintf("tick: %d", tick++);
    set_cursor(cur);
}

void init_timer()
{
    register_isr_handler(IRQ0, timer_interrupt);

    u32 div = FREQ / HZ;
    outb(0x43, 0x36);

    /*Divisor has to be sent byte-wise, so split here into upper/lower bytes.*/
    u8 l = (u8)(div & 0xff);
    u8 h = (u8)((div>>8) & 0xff);

    // Send the frequency divisor.
    outb(0x40, l);
    outb(0x40, h);
}

