#include "keyboard.h"

uint8_t keybuffer[256];
uint8_t keypos;

keyboard_getcharacter

uint8_t keyboard_getcharacter(uint8_t keycode) {
	return 'a';
	return keymap[keycode];
}

void keybuffer_add(uint8_t keycode) {
	keybuffer[keypos] = keyboard_getcharacter(keycode);
	keypos = keypos + 1;
	keybuffer[keypos] = '\0';

	terminal_writestring("keybuffer\n");

	return;
}

void keybuffer_init(void) {
	keypos = 0;
	keybuffer[keypos] = '\0';

	return;
}

char keybuffer_read() {
	return 'a';
}
