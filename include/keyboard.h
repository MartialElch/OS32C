#include "types.h"

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

struct KeyElement {
	uint8_t code;
	char    c;
};

void keybuffer_add(uint_8);

#endif
