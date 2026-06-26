#define VGA_WIDTH            80
#define VGA_HEIGHT           25
#define COLOR                0x0a

void clearScreen(void);
void writeChar(unsigned char);
void writeString(char*);
void scrollScreen(void);
void setCursorPos(uint8_t, uint8_t);
