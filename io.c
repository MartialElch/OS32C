#include <types.h>
#include <io.h>

uint8_t inb(uint16_t port) {
    uint8_t v;

    __asm__ volatile (
        "inb %w1, %b0"
        : "=a" (v)
        : "Nd" (port)
        : "memory");

    return v;
}

void outb(uint16_t port, uint8_t v) {
    __asm__ volatile (
        "outb %b0, %w1"
        : /* no output */
        : "a" (v), "Nd" (port)
        : "memory");
}
