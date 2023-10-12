#include <stdint.h>

/******************************************************************************/
#define COLOR	0x0a

/******************************************************************************/
void terminal_writestring(const char*);

/******************************************************************************/
void kmain(void) {
	terminal_writestring("Hello World");

	while (1) {
	}
}

/******************************************************************************/
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
