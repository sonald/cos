CC=gcc
CFLAGS=-std=gnu99 -Wall -fno-stack-protector -nostdlib -nostdinc -fno-builtin -I./include -m32 -g

objs=boot.o main.o common.o gdt.o isr.o timer.o
all: run

run: kernel
	./mk_img.sh

kernel: $(objs)
	ld -T kernel.ld -melf_i386 -o $@ $^ 

.s.o:
	nasm -f elf32 -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
			
.PHONY: clean

clean:
	-rm *.o 
	-rm kernel
