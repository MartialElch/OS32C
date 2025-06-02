#include <types.h>
#include <io.h>
#include <keyboard.h>
#include <keymap.h>

char keybuffer[KEYBUFFER_MAX];
int keybuffer_read_pos;
int keybuffer_write_pos;

void keyboardBufferAdd(uint8_t c) {
    if (! (c & 0x80)) {    /* ignore key release */
        keybuffer[keybuffer_write_pos++] = keymap[c];
    }

    return;
}

void keyboardInit(void) {
    keybuffer_read_pos = 0;
    keybuffer_write_pos = 0;

    return;
}

void key_handler(void) {
    char a, c;

    __asm__ volatile (
        "    pusha"
        : /* no output */
        : /* no input */);

    c = inb(0x60);
    a = inb(0x61);
    outb(0x61, a & 0x80);
    outb(0x61, a);
    outb(0x20, 0x20);

    keyboardBufferAdd(c);

    __asm__ volatile (
        "popa\n\t"
        "leave\n\t"
        "iret"
        : /* no output */
        : /* no input */);
}
