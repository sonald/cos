; This is the kernel's entry point. We could either call main here,
; or we can use this to setup the stack or other nice stuff, like
; perhaps setting up the GDT and segments. Please note that interrupts
; are disabled at this point: More on interrupts later!
[BITS 32]
global start
global gdt_flush
global idt_flush
extern isr_handler
extern main

; This part MUST be 4byte aligned, so we solve that issue using 'ALIGN 4'
ALIGN 4
mboot:
    ; Multiboot macros to make a few lines later more readable
    MULTIBOOT_PAGE_ALIGN    equ 1<<0
    MULTIBOOT_MEMORY_INFO   equ 1<<1
    MULTIBOOT_AOUT_KLUDGE   equ 1<<16
    MULTIBOOT_HEADER_MAGIC  equ 0x1BADB002
    MULTIBOOT_HEADER_FLAGS  equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE
    MULTIBOOT_CHECKSUM  equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
    EXTERN code, bss, end

    ; This is the GRUB Multiboot header. A boot signature
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM
    
    ; AOUT kludge - must be physical addresses. Make a note of these:
    ; The linker script fills in the data for these ones!
    dd mboot
    dd code
    dd bss
    dd end
    dd start

start:
    cli

    push ebx
    call main

    jmp $

gdt_flush:
    mov eax, [esp+4]
    lgdt [eax]

    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov gs, ax
    mov fs, ax
    mov es, ax
    jmp 0x08:.flush
.flush:
    ret

idt_flush:
    mov eax, [esp+4]
    lidt [eax]
    ret

%macro isr_noerrcode 1
global isr%1
isr%1:
    cli ; seems unneccesary due to int-gate
    push 0
    push %1
    jmp isr_stub
%endmacro

%macro isr_errcode 1
global isr%1
isr%1:
    cli ; seems unneccesary due to int-gate
    push %1
    jmp isr_stub
%endmacro

isr_noerrcode 0
isr_noerrcode 1
isr_noerrcode 2
isr_noerrcode 3
isr_noerrcode 4
isr_noerrcode 5
isr_noerrcode 6
isr_noerrcode 7
isr_errcode   8
isr_noerrcode 9
isr_errcode   10
isr_errcode   11
isr_errcode   12
isr_errcode   13
isr_errcode   14
isr_noerrcode 15
isr_noerrcode 16
isr_noerrcode 17
isr_noerrcode 18
isr_noerrcode 19
isr_noerrcode 20
isr_noerrcode 21
isr_noerrcode 22
isr_noerrcode 23
isr_noerrcode 24
isr_noerrcode 25
isr_noerrcode 26
isr_noerrcode 27
isr_noerrcode 28
isr_noerrcode 29
isr_noerrcode 30
isr_noerrcode 31


isr_stub:
    pusha ; this will do pushad

    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    ;mov ss, ax ; why not ss
    mov gs, ax
    mov fs, ax

    call isr_handler

    pop eax
    mov ds, ax
    mov es, ax
    ;mov ss, ax ; why not ss
    mov gs, ax
    mov fs, ax

    popa
    add esp, 8
    sti
    iret

