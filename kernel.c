#include <types.h>
#include <terminal.h>

void shell(void);
void systemhalt(void);

void kmain(void) {
    clearScreen();

    printString("starting kernel ...\n");
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
