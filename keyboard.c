#include "keyboard.h"
#include "terminal.h"

static char keymap[] = {
	'\0', '\0', '1',  '2',  '3',  '4',  '5',  '6',		// 00
	'7',  '8',  '9',  '\0', '\0', '\0', '\0', '\0',		// 08
	'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',		// 10
	'o',  'p',  '\0', '\0', '\n', '\0', 'a',  's',		// 18
	'd',  'f',  'g',  'h',  'j',  'k',  'l',  '\0',		// 20
	'\0', '\0', '\0', '\0', 'z',  'x',  'c',  'v',		// 28
	'b',  'n',  'm',  '\0', '\0', '\0', '\0', '\0',		// 30
	'\0', ' ' , '\0', '\0', '\0', '\0', '\0', '\0',		// 38
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',		// 40
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',		// 48
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',		// 50
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',		// 58
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',		// 60
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',		// 68
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',		// 70
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'		// 78
};

static char keybuffer[KEYBUFFERSIZE];
static int keyboard_readpos;
static int keyboard_writepos;

char keybuffer_getchar(void) {
	if (keyboard_writepos > keyboard_readpos) {
		return keybuffer[keyboard_readpos];
		keyboard_readpos = keyboard_readpos + 1;
	} else {
		return 0;
	}
}

void keybuffer_write(uint8_t keycode) {
	if (!(keycode&0x80)) {	// ignore key release
		if (keymap[keycode] != '\0') {
			keybuffer[keyboard_writepos] = keymap[keycode];
			keyboard_writepos = keyboard_writepos + 1;
			terminal_putchar(keymap[keycode]);
			terminal_refresh();
		}
	}

	return;
}

void keybuffer_init(void) {
	keyboard_readpos = 0;
	keyboard_writepos = 0;

	return;
}
