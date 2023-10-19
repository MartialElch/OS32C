#include "types.h"

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

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
}

void keybuffer_add(uint8_t);

#endif
