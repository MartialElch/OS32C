#include "types.h"

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYBUFFERSIZE	256

char keybuffer_getchar(void);
void keybuffer_write(uint8_t);

#endif
