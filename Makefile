ASFLAGS = -l $@.lst
CFLAGS  = -Wall -Wextra -Werror -nostdlib -m32 -march=i386 \
          -O0 -Xassembler -al=$*.lst -std=c99
LDFLAGS = --oformat=binary -Ttext=0x10000 -defsym _start=kmain \
          -melf_i386

AS = nasm
CC = gcc
LD = ld

OBJS = kernel.o
BINS = boot.bin kernel.bin

all:	boot.flp

kernel.bin:	$(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

boot.bin:	boot.asm
	$(AS) $< -f bin -o $@

boot.flp:	$(BINS)
	dd if=/dev/zero of=boot.flp bs=512 count=2880
	dd if=boot.bin of=boot.flp bs=512 conv=notrunc
	dd if=kernel.bin of=boot.flp bs=512 seek=1 conv=notrunc

clean:
	rm -f $(BINS) $(OBJS)
	rm -f boot.flp
	rm -f *.lst
