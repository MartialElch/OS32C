#include <types.h>
#include <terminal.h>

#define PROMPT  "c:> "

void shell(void) {
    printString(PROMPT);
    printString("\n");
    printString("exit shell\n");

    return;
}