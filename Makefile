ASFLAGS = -l $(basename $@).lst
CFLAGS  = -std=c89 -O0 -m32 -mtune=pentium \
          -Wall -Wextra -Werror \
          -nostdinc -nostdlib -fno-builtin \
          -nostartfiles -nodefaultlibs \
          -Xassembler -al=$(basename $@).lst \
          -Iinclude
LDFLAGS = --oformat=binary -melf_i386 -Ttext=0x10000 -defsym _start=kmain

AS = nasm
CC = /opt/cross/bin/i386-linux-gcc
LD = /opt/cross/bin/i386-linux-ld

OBJS = head.o kernel.o irq.o irqstubtable.o keyboard.o floppy.o terminal.o io.o shell.o lib.o

all:	boot.flp

kernel.bin:	$(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

boot:	boot.asm
	$(AS) $(ASFLAGS) -o $@ $<

head.o:	head.asm
	$(AS) $(ASFLAGS) -f elf32 $< -o $@

irqstubtable.o:	irqstubtable.asm
	$(AS) $(ASFLAGS) -f elf32 $< -o $@

boot.flp:	boot kernel.bin
	dd if=/dev/zero of=boot.flp bs=512 count=2880
	dd if=boot of=boot.flp bs=512 conv=notrunc
	dd if=kernel.bin of=boot.flp bs=512 seek=1 conv=notrunc

clean:
	rm -f boot kernel.bin boot.flp
	rm -f *.o *.lst
