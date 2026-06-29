#include <types.h>
#include <terminal.h>
#include <lib.h>
#include <irq.h>
#include <keyboard.h>

/* DEBUG */
#include <floppy.h>
void dump(uint32_t addr, uint16_t n);

void shell(void);
void systemhalt(void);

void kmain(void) {
	/* DEBUG */
	char buffer[512];

    clearScreen();

    printf("starting kernel ...\n");

    initIRQ();
    registerIRQ(IRQ_KEYBOARD, &keyboardHandler);
    registerIRQ(IRQ_FLOPPY, &floppyHandler);

    keyboardInit();

	/* DEBUG */
	printf("[DEBUG] IRQ_KEYBOARD = 0x%x\n", IRQ_KEYBOARD);
	printf("[DEBUG] IRQ_FLOPPY = 0x%x\n", IRQ_FLOPPY);

	floppyReset();
	floppyInit();
	floppyReset();
    floppyRead(0x0, (uint32_t)buffer, 32);
    dump((uint32_t)buffer, 32);

    shell();

    systemhalt();
}

void systemhalt(void) {
	printf("system halted\n");

	__asm__ volatile (
		"cli\n"
		"hlt"
		: /* no output */
		: /* no input */);

	return;
}
