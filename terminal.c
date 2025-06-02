#include <types.h>
#include <terminal.h>
#include <io.h>

static int pos, line;

void clearScreen(void) {
    int i, j;

    pos = 0;
    line = 0;

    for (i=0; i<25; i++) {
        for (j=0; j<80; j++) {
            printChar(' ');
        }
    }

    pos = 0;
    line = 0;

    return;
}

void printChar(unsigned char c) {
    volatile unsigned char *videoram = (unsigned char *)0xb8000;

    if (c == '\r') {
        pos = 0;
    } else if (c == '\n') {
        pos = 0;
        line++;
    } else {         /* printable char */
        videoram[2*(80*line + pos)] = c;
        videoram[2*(80*line + pos) + 1] = COLOR;
        pos++;
    }

    if (pos == VGA_WIDTH) {
        pos = 0;
        line++;
    }

    if (line == VGA_HEIGHT) {
        scrollScreen();
        line--;
    }

    setCursorPos(pos, line);

    return;
}

void printString(char *s) {
    while (*s != '\0') {
        printChar(*s);
        s++;
    }

    return;
}

void scrollScreen(void) {
    volatile unsigned char *videoram = (unsigned char *)0xb8000;
    int row, col;

    for (row=1; row<VGA_HEIGHT; row++) {
        for (col=0; col<VGA_WIDTH; col++) {
            videoram[2*(80*(row-1) + col)]     = videoram[2*(80*row + col)];
            videoram[2*(80*(row-1) + col) + 1] = videoram[2*(80*row + col) + 1];
        }
    }

    /* blank out last line */
    for (col=0; col<VGA_WIDTH; col++) {
        videoram[2*(80*(VGA_HEIGHT-1) + col)] = ' ';
        videoram[2*(80*(VGA_HEIGHT-1) + col) + 1] = COLOR;
    }

    return;
}

void setCursorPos(uint8_t cursorPositionX, uint8_t cursorPositionY) {
    uint16_t pos = cursorPositionY * VGA_WIDTH + cursorPositionX;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));

    return;
}
