#include "terminal.h"

void shell() {
	static char s[80];
	char c;
	int i;

	// initialize
	i = 0;
	s[0] = '\0';

	terminal_writestring("c:> ");

	while (1) {
		if ((c = sys_getchar())) {
			if (c == '\n') {
				terminal_writestring("\n");
				terminal_writestring("evaluate\n");
				terminal_writestring("c:> ");
			} else {
				terminal_putchar(c);
				i = i + 1;
			}
		}
	}

	return;
}