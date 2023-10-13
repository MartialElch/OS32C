/******************************************************************************/
#include <stdint.h>

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
void terminal_writestring(const char*);

/******************************************************************************/
void kmain(void) {
	terminal_writestring("Hello World");

	while (1) {
	}
}

/******************************************************************************/
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
}

void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

/******************************************************************************/
