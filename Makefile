ASFLAGS = -l $(basename $@).lst

AS = nasm

all:	boot.flp

boot:	boot.asm
	$(AS) $(ASFLAGS) -o $@ $<

boot.flp:	boot
	dd if=/dev/zero of=boot.flp bs=512 count=2880
	dd if=boot of=boot.flp bs=512 conv=notrunc

clean:
	rm -f boot boot.flp
