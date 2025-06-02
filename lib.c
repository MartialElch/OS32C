#include <types.h>
#include <lib.h>
#include <keyboard.h>
#include <terminal.h>
#include <system.h>

extern char keybuffer[];
extern int keybuffer_read_pos;
extern int keybuffer_write_pos;

void exit(__attribute__ ((unused)) int status){
    systemhalt();
    return;
}

int getchar(void) {
    int c;

    while (keybuffer_read_pos == keybuffer_write_pos) {
        __asm__ volatile (
            "hlt"
        );
    }

    c = keybuffer[keybuffer_read_pos++];

    if (keybuffer_read_pos == KEYBUFFER_MAX) {
        keybuffer_read_pos = 0;
    }

    return c;
}

int putchar(int c) {
    printChar(c);

    return c;
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