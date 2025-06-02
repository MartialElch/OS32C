#define VGA_WIDTH            80
#define VGA_HEIGHT           25
#define COLOR                0x0a

void clearScreen(void);
void printChar(unsigned char);
void printString(char*);
void setCursorPos(uint8_t, uint8_t);
