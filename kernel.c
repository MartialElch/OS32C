#include <types.h>
#include <terminal.h>
#include <lib.h>
#include <irq.h>
#include <keyboard.h>

void shell(void);
void systemhalt(void);

void kmain(void) {
    clearScreen();

    printf("starting kernel ...\n");

	printf("[DEBUG] 1234 = %d\n", 1234);
	printf("[DEBUG] 0x12dead56 = %x\n", 0x12DEAD56);

    initIRQ();
    registerIRQ(IRQ_KEYBOARD, &key_handler);

    keyboardInit();

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
