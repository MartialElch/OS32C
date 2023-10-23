#include "terminal.h"

void evaluate(char *s) {
	terminal_writestring(s);
	terminal_writestring("\n");

	return;
}

void reset_buffer() {
	return;
}

void shell() {
	static char s[80];
	char c;
	int i;

	// initialize
	i = 0;
	s[i] = '\0';

	terminal_writestring("c:> ");

	while (1) {
		if ((c = sys_getchar())) {
			if (c == '\n') {
				terminal_writestring("\n");
				terminal_writestring("evaluate\n");
				evaluate(s);
				terminal_writestring("c:> ");
			} else {
				s[i++] = c;
				s[i] = '\0';
				terminal_putchar(c);
				terminal_refresh();
			}
		}
	}

	return;
}