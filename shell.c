#include <types.h>
#include <terminal.h>
#include <lib.h>

#define MAX_CMD 80
#define PROMPT  "c:> "

void execute(char *cmd);

void execute(char *cmd) {
    if (strcmp(cmd, "exit") == 0) {
        exit(0);
    } else if (strcmp(cmd, "") == 0) {
    } else {
        printString(cmd);
        printString(" not found\n");
    }

    return;
}

void shell(void) {
    static char cmd[MAX_CMD];
    char c;
    int i = 0;

    cmd[0] = '\0';

    printString(PROMPT);

    while ((c = getchar())) {
        putchar(c);
        if (c == '\n') {
            execute(cmd);
            cmd[0] = '\0';
            i = 0;
            printString(PROMPT);
        } else {
            cmd[i++] = c;
            cmd[i] = '\0';        }
    }

    return;
}