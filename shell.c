#include <types.h>
#include <terminal.h>
#include <lib.h>
#include <floppy.h>

#define MAX_CMD 80
#define PROMPT  "c:> "

void dump(uint32_t addr, uint16_t n);
void execute(char *cmd);

void dir() {
    char buffer[512];

    floppyRead(0x0, (uint32_t)buffer, 32);
    dump((uint32_t)buffer, 32);

    return;
}

void dump(uint32_t addr, uint16_t n) {
    volatile uint8_t* mem;
    uint32_t i;

	mem = 0;
	for (i=0; i<n; i++) {
		if ((i%16) == 0) {
			if ((i > 0) & (i<n)) {
				printf("\n");
			}
			printf("000000  ");
		}
		if ((i%16) == 8) {
			printf(" ");
		}
		printf("%02x ", mem[addr+i]);
	}
	printf("\n");

    return;
}

void execute(char *cmd) {
    if (strcmp(cmd, "clear") == 0) {
        clearScreen();
    } else if (strcmp(cmd, "dir") == 0) {
        dir();
    } else if (strcmp(cmd, "dump") == 0) {
        dump(0x4000, 32);
    } else if (strcmp(cmd, "exit") == 0) {
        exit(0);
    } else if (strcmp(cmd, "") == 0) {
    } else {
        printf(cmd);
        printf(" not found\n");
    }

    return;
}

void shell(void) {
    static char cmd[MAX_CMD];
    char c;
    int i = 0;

    cmd[0] = '\0';

    printf(PROMPT);

    while ((c = getchar())) {
        putchar(c);
        if (c == '\n') {
            execute(cmd);
            cmd[0] = '\0';
            i = 0;
            printf(PROMPT);
        } else {
            cmd[i++] = c;
            cmd[i] = '\0';        }
    }

    return;
}