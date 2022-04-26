CFLAGS  = -std=gnu99 -nostdlib -m32 -march=i386 -ffreestanding -fno-asynchronous-unwind-tables
# ASFLAGS = -Wa,-a=kernel.lst
LDFLAGS = -Wl,--nmagic,--script=kernel.ld

AS = nasm
CC = gcc

all:	floppy

floppy:
	dd if=/dev/zero of=floppy.img bs=512 count=2880
	$(AS) boot.asm -f bin -o boot.bin
	dd if=boot.bin of=floppy.img bs=512 conv=notrunc
	$(CC) -o kernel.bin $(CFLAGS) $(ASFLAGS) $(LDFLAGS) kernel.c
	# $(AS) kernel.asm -f bin -o kernel.bin -l kernel.lst
	dd if=kernel.bin of=floppy.img bs=512 seek=1 conv=notrunc

clean:
	rm -f boot.bin kernel.bin floppy.img
	rm -f kernel.lst kernel.o
