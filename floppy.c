#include <types.h>
#include <lib.h>
#include <floppy.h>
#include <irq.h>
#include <io.h>

void floppyCommand(uint8_t cmd) {
	int i;
	for (i=0; i<10; i++) {
		if (0x80 & inb(MAIN_STATUS_REGISTER)) {
			outb(cmd, DATA_FIFO);
 			return;
		}
	}
}

void floppyHandler(void) {
    __asm__ volatile (
        "    pusha"
        : /* no output */
        : /* no input */);

     outb(0x20, 0x20);
   __asm__ volatile (
        "popa\n\t"
        "leave\n\t"
        "iret"
        : /* no output */
        : /* no input */);
}

void floppyInit() {
	printf("init floppy\n");

	printf("MSR = 0x%x\n", inb(MAIN_STATUS_REGISTER));
	printf("DOR = 0x%x\n", inb(DIGITAL_OUTPUT_REGISTER));

	floppyCommand(VERSION);
	timerSleep(1);
	printf("DATA_FIFO = 0x%x\n", inb(DATA_FIFO));

	floppyCommand(CONFIGURE);
	floppyCommand(0);
	floppyCommand(0x57);
	floppyCommand(0);
	floppyCommand(LOCK);
	timerSleep(10);

	return;
}

void floppyRead(uint32_t addr_src, uint32_t addr_tgt, uint32_t len) {
	uint8_t sector, track, head, drive;

	union addr {
		uint8_t b[4];	/* 4 bytes */
		uint32_t l;		/* 1 32-bit */
	} a, c;				/* address and count */

	printf("read floppy\n");

	a = (union addr)addr_tgt;
	c = (union addr)(len-1);

	outb(0x06, 0x0a);				/* mask chan 2 */
	outb(0xff, 0x0c);				/* reset flip-flop */
	outb(a.b[0], 0x04);				/* address low byte */
	outb(a.b[1], 0x04);				/* address high byte */
	outb(a.b[2], 0x81);				/* external page register */
	outb(0xff, 0x0c);				/* reset flip-flop */
	outb(c.b[0], 0x05);				/* count low byte */
	outb(c.b[1], 0x05);				/* count high byte */
	outb(0x46, 0x0b);				/* set mode read */
	outb(0x02, 0x0a);				/* unmask chan 2 */

	sector = (addr_src/512)%18 + 1;	/* sector, numbering starts with 1 */
	track  = (addr_src/512)/(2*18);	/* track, alternating between heads */
	head   = ((addr_src/512)/18)%2;
	drive  = 1;						/* drive B: */

	floppyCommand(READ_DATA);		/* read command */
	floppyCommand(head<<2 | drive);	/* head and drive */
	floppyCommand(track);			/* cylinder */
	floppyCommand(head);			/* first head */
	floppyCommand(sector);			/* first sector, counts from 1 */
	floppyCommand(2);				/* bytes per sector 128*2^n */
	floppyCommand(18);				/* number of tracks to operate on */
	floppyCommand(0x1b);			/* GAP3 length, 27 is default */
	floppyCommand(0xff);			/* data length */
	waitIRQ(IRQ_FLOPPY);

	printf("read done\n");
}

void floppyReset(void) {
	printf("reset floppy\n");

	outb(0x00, DIGITAL_OUTPUT_REGISTER);	/* disable controller */
	timerSleep(1);
	outb(0x1D, DIGITAL_OUTPUT_REGISTER);	/* enable controller */
	printf("wait for interrupt\n");

	return;
}
