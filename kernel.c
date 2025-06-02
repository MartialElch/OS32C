#include <types.h>
#include <terminal.h>
#include <irq.h>
#include <keyboard.h>

void shell(void);
void systemhalt(void);

void kmain(void) {
    clearScreen();

    printString("starting kernel ...\n");

    initIRQ();
    registerIRQ(IRQ_KEYBOARD, &key_handler);

    keyboardInit();

    shell();

    systemhalt();
}

void systemhalt(void) {
	printString("system halted\n");

	__asm__ volatile (
		"cli\n"
		"hlt"
		: /* no output */
		: /* no input */);

	return;
}
