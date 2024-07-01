ASFLAGS = -l $@.lst

AS = nasm

all:	boot.flp

boot.bin:	boot.asm
	$(AS) $< -f bin -o $@

boot.flp:	boot.bin
	dd if=/dev/zero of=boot.flp bs=512 count=2880
	dd if=boot.bin of=boot.flp bs=512 conv=notrunc

clean:
	rm -f boot.bin boot.flp
