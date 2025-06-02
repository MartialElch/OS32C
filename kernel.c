typedef unsigned char			uint8_t;
typedef unsigned short int		uint16_t;

#define VGA_WIDTH            80
#define VGA_HEIGHT           25
#define COLOR                0x0a

void clearScreen(void);
void printChar(unsigned char);
void printString(char*);
void setCursorPos(uint8_t, uint8_t);
void systemhalt(void);

static int pos, line;

void kmain(void) {
    clearScreen();

    printString("starting kernel ...\n");

    systemhalt();
}

void systemhalt(void) {
	printString("system halted\n");

	__asm__ volatile (
		"cli\n"
		"hlt"
		: /* no output */
		: /* no input */);

	return;
}

/******************************************************************************/
/* io */
uint8_t inb(uint16_t port) {
    uint8_t v;

    __asm__ volatile (
        "inb %w1, %b0"
        : "=a" (v)
        : "Nd" (port)
        : "memory");

    return v;
}

void outb(uint16_t port, uint8_t v) {
    __asm__ volatile (
        "outb %b0, %w1"
        : /* no output */
        : "a" (v), "Nd" (port)
        : "memory");
}

/******************************************************************************/
/* terminal */
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

void setCursorPos(uint8_t cursorPositionX, uint8_t cursorPositionY) {
    uint16_t pos = cursorPositionY * VGA_WIDTH + cursorPositionX;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));

    return;
}

/******************************************************************************/
