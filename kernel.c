/******************************************************************************/
#include "irq.h"
#include "types.h"
#include "keyboard.h"
#include "shell.h"
#include "terminal.h"

/******************************************************************************/
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

#define COLOR	0x0a

/******************************************************************************/
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;

volatile uint16_t* terminal_buffer;
volatile uint16_t VGA_BUFFER[VGA_WIDTH*VGA_HEIGHT];

/******************************************************************************/
/* kernel */
void system_halt(void);

void kmain(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = COLOR;

	terminal_buffer = (uint16_t*) 0xb8000;

	terminal_writestring("start kernel\n");

	irq_init();
	irq_register(IRQ_KEYBOARD+IRQ_BASE, &key_handler, TYPE_IRQ);
	irq_enable();

	shell();

	system_halt(); // exit message and stop system
}

void system_halt() {
	terminal_writestring("stop system\n");
	__asm volatile("cli;hlt");
}

/******************************************************************************/
/* terminal handling */

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len]) {
		len++;
	}
	return len;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);

	VGA_BUFFER[index] = vga_entry(c, color);
}

void terminal_refresh(void) {
	size_t index;

	for (int i=0; i<VGA_HEIGHT; i++) {
		for (int j=0; j<VGA_WIDTH; j++) {
			index = i * VGA_WIDTH + j;
			terminal_buffer[index] = VGA_BUFFER[index];
		}
	}
}

void terminal_scroll(void) {
	for (int i=0; i<VGA_HEIGHT-1; i++) {
		for (int j=0; j<VGA_WIDTH; j++) {
			VGA_BUFFER[i*VGA_WIDTH + j] = VGA_BUFFER[(i+1)*VGA_WIDTH + j];
		}
	}

	for (int i=0; i<VGA_WIDTH; i++) {
		VGA_BUFFER[(VGA_HEIGHT-1)*VGA_WIDTH + i] = vga_entry(' ', terminal_color);
	}

	terminal_refresh();
}

void terminal_putchar(char c) {
	if (c == '\n') {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) {
			// terminal_row = 0;
			terminal_scroll();
			terminal_row = VGA_HEIGHT-1;
		}
	} else {
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

		if (++terminal_column == VGA_WIDTH) {
			terminal_column = 0;
			if (++terminal_row == VGA_HEIGHT) {
				// terminal_row = 0;
				terminal_scroll();
				terminal_row = VGA_HEIGHT-1;
			}
		}
	}
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		terminal_putchar(data[i]);	
	}

	terminal_refresh();
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

/******************************************************************************/
/* IRQ handling */

static inline uint8_t inb(uint16_t port) {
	uint8_t v;

	__asm volatile (
		"inb %1, %0"
		: "=a" (v)
		: "dN" (port)
		: /* no globber */);

	return v;
}
static inline void outb(uint8_t v, uint16_t port) {
	__asm volatile (
		"outb %0, %1"
		: /* no output */
		: "a" (v), "dN" (port)
		: /* no globber */);

	return;
}

void irq_setidt(void *base, unsigned int limit) {
   unsigned int i[2];

   i[0] = limit << 16;
   i[1] = (unsigned int) base;
   __asm volatile (
		"	lidt (%0)"
		: /* no output */
		: "p" (((char *) i)+2)
		: /* no globber */);
}

void irq_init(void) {
	irq_setidt((void*)idt, 256*8-1);

	return;
}

void irq_enable(void) {
	__asm volatile (
		"	sti"
		: /* no output */
		: /* no input */
		: /* no globber */);
}

void irq_register(int n, void* addr, uint8_t type) {
	terminal_writestring("register IRQ\n");

	idt[n].offset_1 = (uint32_t)addr & 0xffff;
	idt[n].offset_2 = ((uint32_t)addr >> 16) & 0xffff;
	idt[n].selector = 0x08;
	idt[n].type_attr = type;
	idt[n].zero = 0x00;
}

void key_handler(void) {
	uint8_t c, k;

	__asm ("pusha");
	// terminal_writestring("key_handler entry\n");
	k = inb(0x60);
	c = inb(0x61);
	outb(c | 0x80, 0x61);
	outb(c, 0x61);
	outb(0x20, 0x20);
	// terminal_writestring("call keybuffer_write\n");
	keybuffer_write(k);
	// terminal_writestring("key_handler done\n");
	__asm ("popa; leave; iret");
}

/******************************************************************************/
/* system calls */

char sys_getchar(void) {
	return keybuffer_getchar();
}

/******************************************************************************/
