#include "terminal.h"

int strcmp(const char*, const char*);

static char buffer[80];
int buffer_pos = 0;

void cmd_dir() {
}

void cmd_halt() {
	terminal_writestring("system halt\n");
	__asm volatile("cli; hlt");
}

void evaluate(char *cmd) {
	if (strcmp(cmd, "start") == 0) {
		terminal_writestring("CMD: start\n");
	} else if (strcmp(cmd, "dir") == 0) {
		terminal_writestring("CMD: dir\n");
		cmd_dir();
	} else if (strcmp(cmd, "halt") == 0) {
		terminal_writestring("CMD: halt\n");
		cmd_halt();
        } else {
		terminal_writestring(cmd);
		terminal_writestring(" not found\n");
        }

	return;
}

void buffer_add(char c) {
	buffer[buffer_pos] = c;
	buffer[buffer_pos+1] = '\0';
	buffer_pos++;
}

void buffer_reset() {
	buffer[0] = '\0';
	buffer_pos = 0;

	return;
}

int strcmp(const char *s1, const char *s2) {
        char c;
        int rc = 0;
        int i=0;

        while((c = s1[i]) != '\0') {
                if (s2[i] == '\0') {
                        rc = -1;
                        break;
                } else if (s2[i] != c) {
                        rc = -1;
                        break;
                }
                i++;
        }

        if (s2[i] != '\0') {
                rc = 1;
        }

        return rc;
}

void shell() {
	char c;

	// initialize
	buffer_reset();
	terminal_writestring("c:> ");

	while (1) {
		if ((c = sys_getchar())) {
			if (c == '\n') {
				terminal_writestring("\n");
				terminal_writestring("evaluate\n");
				evaluate(buffer);
				buffer_reset();
				terminal_writestring("c:> ");
			} else {
				buffer_add(c);
				terminal_putchar(c);
				terminal_refresh();
			}
		}
	}

	return;
}
