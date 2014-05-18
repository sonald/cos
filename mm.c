#include "mm.h"
#include "isr.h"
#include "kheap.h"

extern u32 _placement_addr;

static page_directory_t* kernel_directory = NULL;
static page_directory_t* current_directory = NULL;

static u32* frames = NULL;
static u32 nframes = 0;

// before we can get system mem size, assume 16MB
u32 mem_end_page = 0x1000000;

static void page_fault(registers_t regs)
{
    // read cr2
    u32 fault_addr;
    __asm__ __volatile__ ("movl %%cr2, %0":"=r"(fault_addr));
    int present = !(regs.errcode & 0x1); // Page not present
    int rw = regs.errcode & 0x2;           // Write operation?
    int us = regs.errcode & 0x4;           // Processor was in user-mode?
    int rsvd = regs.errcode & 0x8; // reserved bit in pd set

    kprintf("PF: addr 0x%x, %s, %c, %c, %s\n", fault_addr, 
            (present?"P":"NP"), (rw?'W':'R'), (us?'U':'S'), (rsvd?"RSVD":""));
    PANIC("page_fault");
}

static void enable_paging()
{
    u32 cr0;
    __asm__ __volatile__ ("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ __volatile__ ("mov %0, %%cr0":: "r"(cr0));
}

static void setup_pages()
{
    kprintf("total mem: %dMB\n", mem_end_page / 1024);
    nframes = mem_end_page / 4; // mem_end_page unit is KB
    frames = (u32*)kmalloc(nframes / 8);
    memset(frames, 0, nframes / 8);

    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));

    kprintf("frames: %d, _placement_addr: 0x%x\n", nframes, _placement_addr);
    // do identity mapping for kernel space now.
    u32 i = 0;
    while (i < _placement_addr) {
        alloc_frame(get_page(i, 1, kernel_directory), 1, 0);
        i += 0x1000;
    }

    kprintf("frames: %d, _placement_addr: 0x%x\n", nframes, _placement_addr);
    switch_page_directory(kernel_directory);
}

extern u32 _end;
void init_mm()
{
    register_isr_handler(PAGE_FAULT, page_fault);
    _placement_addr = (u32)&_end; // heap is right after .BSS
    setup_pages();
    enable_paging();
}


static void set_frame(u32 frame_addr)
{
    u32 fid = frame_addr >> 12;
    frames[fid/32] |= (1<<(fid%32));
}

static void clear_frame(u32 frame_addr)
{
    u32 fid = frame_addr >> 12;
    frames[fid/32] &= ~(1<<(fid%32));
}

static int test_frame(u32 frame_addr)
{
    u32 fid = frame_addr >> 12;
    return frames[fid/32] & (1<<(fid%32));
}

static u32 get_first_frame()
{
    for (u32 i = 0; i < nframes / 32; ++i) {
        if (frames[i] != 0xffffffff) {
            for (u8 j = 0; j < 32; ++j) {
                if (!(frames[i] & (1<<j))) {
                    return i*32+j;
                }
            }
        }
    }
    //panic here?
    return (u32)-1;
}

void switch_page_directory(page_directory_t *pgdir)
{
    if (current_directory != pgdir) {
        current_directory = pgdir;
        __asm__ __volatile__ ("mov %0, %%cr3" :: "r"(&pgdir->table_physicals));
    }
}

// map a linear address into a page entry at dir
page_t *get_page(u32 vaddr, int make, page_directory_t *dir)
{
    vaddr /= 0x1000; // into a index
    int tid = vaddr / 1024;
    page_t *pg = NULL;
    if (dir->tables[tid]) {
        pg = &dir->tables[tid]->pages[vaddr%1024];

    } else if (make) {
        u32 phys = 0;
        dir->tables[tid] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &phys);
        memset(dir->tables[tid], 0, sizeof(page_table_t));

        dir->table_physicals[tid] = (phys & 0xfffff000) | 0x7; //Present, RW, US
        pg = &dir->tables[tid]->pages[vaddr%1024];
    }
    /*kprintf("GP: 0x%x, 0x%x; ", tid, vaddr%1024);*/
    return pg;
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
    if (page->frame) {
        return;
    }

    u32 fid = get_first_frame();
    if (fid == (u32)-1) {
        PANIC("no free frames available!");
    }
    set_frame(fid<<12);
    page->frame = fid;
    page->present = 1;
    page->rw = (is_writeable ? 1 : 0);
    page->user = (is_kernel ? 0: 1);
}

void free_frame(page_t *page)
{
    if (page->frame) {
        clear_frame(page->frame);
        page->frame = 0;
        page->present = 0;
    }
}

