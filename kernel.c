/******************************************************************************/
asm (
	".align 4\n"
	"call kmain\n");

/******************************************************************************/
#include <stddef.h>
#include "include/types.h"
#include "include/irq.h"

/******************************************************************************/
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

/******************************************************************************/
void terminal_writebyte(const uint8_t);
void terminal_writestring(const char*);

/******************************************************************************/
enum Interrupts
{
	IRQ_BASE     = 32,
	IRQ_KEYBOARD = 1,
	IRQ_FLOPPY   = 6
};

enum FloppyRegisters
{
	STATUS_REGISTER_A                = 0x3F0,	// read-only
	STATUS_REGISTER_B                = 0x3F1,	// read-only
	DIGITAL_OUTPUT_REGISTER          = 0x3F2,
	TAPE_DRIVE_REGISTER              = 0x3F3,
	MAIN_STATUS_REGISTER             = 0x3F4,	// read-only
	DATARATE_SELECT_REGISTER         = 0x3F4,	// write-only
	DATA_FIFO                        = 0x3F5,
	DIGITAL_INPUT_REGISTER           = 0x3F7,	// read-only
	CONFIGURATION_CONTROL_REGISTER   = 0x3F7	// write-only
};

enum FloppyCommands
{
	READ_TRACK =         2,		// generates IRQ6
	SPECIFY =            3,		// * set drive parameters
	SENSE_DRIVE_STATUS = 4,
	WRITE_DATA =         5,		// * write to the disk
	READ_DATA =          6,		// * read from the disk
	RECALIBRATE =        7,		// * seek to cylinder 0
	SENSE_INTERRUPT =    8,		// * ack IRQ6, get status of last command
	WRITE_DELETED_DATA = 9,
	READ_ID =            10,	// generates IRQ6
	READ_DELETED_DATA =  12,
	FORMAT_TRACK =       13,	// *
	SEEK =               15,	// * seek both heads to cylinder X
	VERSION =            16,	// * used during initialization, once
	SCAN_EQUAL =         17,
	PERPENDICULAR_MODE = 18,	// * used during initialization, once, maybe
	CONFIGURE =          19,	// * set controller parameters
	LOCK =               20,	// * protect controller params from a reset
	VERIFY =             22,
	SCAN_LOW_OR_EQUAL =  25,
	SCAN_HIGH_OR_EQUAL = 29
};

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK =         0,
	VGA_COLOR_BLUE =          1,
	VGA_COLOR_GREEN =         2,
	VGA_COLOR_CYAN =          3,
	VGA_COLOR_RED =           4,
	VGA_COLOR_MAGENTA =       5,
	VGA_COLOR_BROWN =         6,
	VGA_COLOR_LIGHT_GREY =    7,
	VGA_COLOR_DARK_GREY =     8,
	VGA_COLOR_LIGHT_BLUE =    9,
	VGA_COLOR_LIGHT_GREEN =   10,
	VGA_COLOR_LIGHT_CYAN =    11,
	VGA_COLOR_LIGHT_RED =     12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN =   14,
	VGA_COLOR_WHITE =         15,
};

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;

volatile uint16_t* terminal_buffer;
volatile uint8_t* mem;

volatile uint8_t IRQ[16];
volatile uint8_t FLOPPY_BUFFER[512];
volatile uint16_t VGA_BUFFER[VGA_WIDTH*VGA_HEIGHT];


/******************************************************************************/
static inline void outb(uint8_t v, uint16_t port) {
	asm volatile (
		"outb %0, %1"
		: /* no output */
		: "a" (v), "dN" (port)
		: /* no globber */);
}

static inline uint8_t inb(uint16_t port) {
	uint8_t v;
	asm volatile (
		"inb %1, %0"
		: "=a" (v)
		: "dN" (port)
		: /* no globber */);
	return v;
}

void timer_sleep(int t) {
	for (int i=0; i<t; i++) {
		for (int j=0; j<60000000; j++) {
		}
	}
}

void irq_wait(uint8_t irq) {
	while (IRQ[irq] == 0) {
		// wait
		timer_sleep(1);
	} 
	IRQ[irq] = 0;

	terminal_writestring("IRQ ");
	terminal_writebyte(irq);
	terminal_writestring(" received\n");
}

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len]) {
		len++;
	}
	return len;
}

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xb8000;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);

	VGA_BUFFER[index] = vga_entry(c, color);
}

void terminal_refresh(void) {
	size_t index;

	for (int i=0; i<VGA_HEIGHT; i++) {
		for (int j=0; j<VGA_WIDTH; j++) {
			index = i * VGA_WIDTH + j;
			terminal_buffer[index] = VGA_BUFFER[index];
		}
	}
}

void terminal_scroll(void) {
	for (int i=0; i<VGA_HEIGHT-1; i++) {
		for (int j=0; j<VGA_WIDTH; j++) {
			VGA_BUFFER[i*VGA_WIDTH + j] = VGA_BUFFER[(i+1)*VGA_WIDTH + j];
		}
	}

	for (int i=0; i<VGA_WIDTH; i++) {
		VGA_BUFFER[(VGA_HEIGHT-1)*VGA_WIDTH + i] = vga_entry(' ', terminal_color);
	}

	terminal_refresh();
}

void terminal_putchar(char c) {
	if (c == '\n') {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) {
			// terminal_row = 0;
			terminal_scroll();
			terminal_row = VGA_HEIGHT-1;
		}
	} else {
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

		if (++terminal_column == VGA_WIDTH) {
			terminal_column = 0;
			if (++terminal_row == VGA_HEIGHT) {
				// terminal_row = 0;
				terminal_scroll();
				terminal_row = VGA_HEIGHT-1;
			}
		}
	}
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		terminal_putchar(data[i]);	
	}
}

void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

void terminal_writebyte(const uint8_t byte) 
{
	char *hexstring = "0123456789ABCDEF";
	terminal_putchar(hexstring[(byte >> 4) & 0x0f]);
	terminal_putchar(hexstring[byte & 0x0f]);
}

void dump(uint32_t addr, size_t n) {
	uint32_t i;

	mem = 0;
	for (i=0; i<n; i++) {
		if ((i%16) == 0) {
			if ((i > 0) & (i<n)) {
				terminal_writestring("\n");
			}
			terminal_writestring("000000  ");
		}
		if ((i%16) == 8) {
			terminal_writestring(" ");
		}
		terminal_writebyte(mem[addr+i]);
		terminal_writestring(" ");
	}
	terminal_writestring("\n");
}

void fdc_reset(void) {
	terminal_writestring("reset floppy\n");
	// outb(0x80, DATARATE_SELECT_REGISTER);	// reset controller
	// terminal_writestring("wait for interrupt\n");
	// return;

	outb(0x00, DIGITAL_OUTPUT_REGISTER);	// disable controller
	timer_sleep(1);
	outb(0x1C, DIGITAL_OUTPUT_REGISTER);	// enable controller
	terminal_writestring("wait for interrupt\n");
	return;

	outb(SENSE_INTERRUPT, DATA_FIFO);
	if (0x80 & inb(MAIN_STATUS_REGISTER)) {
		terminal_writestring("MSR = 0x");
		terminal_writebyte(inb(MAIN_STATUS_REGISTER));
		terminal_writestring("\n");

		terminal_writestring("st0 = 0x");
		terminal_writebyte(inb(DATA_FIFO));
		terminal_writestring("\n");

		terminal_writestring("cyl = 0x");
		terminal_writebyte(inb(DATA_FIFO));
		terminal_writestring("\n");
	} else {
		terminal_writestring("MSR = 0x");
		terminal_writebyte(inb(MAIN_STATUS_REGISTER));
		terminal_writestring("\n");
	}

	terminal_writestring("reset done\n");
}

void fdc_command(uint8_t cmd) {
	for (int i=0; i<10; i++) {
		if (0x80 & inb(MAIN_STATUS_REGISTER)) {
			outb(cmd, DATA_FIFO);
			// terminal_writestring("loop i = 0x");
			// terminal_writebyte(i & 0xff);
 			// terminal_writestring("\n");
 			return;
		}
	}
}

void fdc_read(uint32_t addr_src, uint32_t addr_tgt, uint32_t len) {
	union addr {
		uint8_t b[4];	// 4 bytes
		uint32_t l;	// 1 32-bit
	} a, c; // address and count

	terminal_writestring("read floppy\n");

	a = (union addr)addr_tgt;
	c = (union addr)(len-1);

	outb(0x06, 0x0a);		// mask chan 2
	outb(0xff, 0x0c);		// reset flip-flop
	outb(a.b[0], 0x04);		// address low byte
	outb(a.b[1], 0x04);		// address high byte
	outb(a.b[2], 0x81);		// external page register
	outb(0xff, 0x0c);		// reset flip-flop
	outb(c.b[0], 0x05);		// count low byte
	outb(c.b[1], 0x05);		// count high byte‚
	outb(0x46, 0x0b);		// set mode read
	outb(0x02, 0x0a);		// unmask chan 2

	fdc_command(READ_DATA);		// read command
	fdc_command(0);			// head and driver
	fdc_command(0);			// cylinder
	fdc_command(0);			// first head
	fdc_command(1);			// first sector, counts from 1
	fdc_command(2);			// bytes per sector 128*2^n
	fdc_command(18);		// number of tracks to operate on
	fdc_command(0x1b);		// GAP3 length, 27 is default
	fdc_command(0xff);		// data length
	irq_wait(IRQ_FLOPPY);

	terminal_writestring("read done\n");
}

void irq_handler(void) {
	asm("pusha");
	terminal_writestring("irq_handler entry\n");

	outb(0x20, 0x20);
	terminal_writestring("irq_handler done\n");
	asm("popa; leave; iret");
}

void fdc_handler(void) {
	asm("pusha");
	// terminal_writestring("fdc_handler entry\n");

	IRQ[IRQ_FLOPPY] = 1;

	outb(0x20, 0x20);
	// terminal_writestring("fdc_handler done\n");
	asm("popa; leave; iret");
}

void key_handler(void) {
	uint8_t c, k;

	asm("pusha");
	terminal_writestring("key_handler entry\n");
	k = inb(0x60);
	terminal_writebyte(k);
	terminal_writestring(" ");
	c = inb(0x61);
	terminal_writebyte(c);
	terminal_writestring(" ");
	terminal_writebyte(c|0x80);
	terminal_writestring("\n");
	outb(c | 0x80, 0x61);
	outb(c, 0x61);
	outb(0x20, 0x20);
	terminal_writestring("key_handler done\n");
	asm("popa; leave; iret");
}

void set_idt(void *base, unsigned int limit) {
   unsigned int i[2];

   i[0] = limit << 16;
   i[1] = (unsigned int) base;
   asm volatile (
		"	lidt (%0)"
		: /* no output */
		: "p" (((char *) i)+2)
		: );
}

void irq_reprogram() {
	set_idt((void*)idt, 256*8-1);

	return;
}

void irq_register(int n, void* addr, uint8_t type) {
	// terminal_writestring("register IRQ ");
	// terminal_writestring("0x");
	// terminal_writebyte(n);
	// terminal_writestring("\n");

	idt[n].offset_1 = (uint32_t)addr & 0xffff;
	idt[n].offset_2 = ((uint32_t)addr >> 16) & 0xffff;
	idt[n].selector = 0x08;
	idt[n].type_attr = type;
	idt[n].zero = 0x00;

	// terminal_writestring("done\n");
}

/******************************************************************************/
/* Kernel */

void kmain(void) {
	terminal_initialize();
	terminal_row = 2;
	terminal_writestring("start kernel\n");

	irq_reprogram();
	for (int i=0; i<256; i++) {
		irq_register(i, &irq_handler, TYPE_IRQ);
	}
	irq_register(IRQ_KEYBOARD+IRQ_BASE, &key_handler, TYPE_IRQ);
	irq_register(IRQ_FLOPPY+IRQ_BASE, &fdc_handler, TYPE_IRQ);
	irq_enable();

	// fdc_init();
	// fdc_reset();

	terminal_writestring("MSR = 0x");
	terminal_writebyte(inb(MAIN_STATUS_REGISTER));
	terminal_writestring("\n");
	terminal_writestring("DOR = 0x");
	terminal_writebyte(inb(DIGITAL_OUTPUT_REGISTER));
	terminal_writestring("\n");

	fdc_command(VERSION);
	timer_sleep(1);
	terminal_writestring("DATA_FIFO = 0x");
	terminal_writebyte(inb(DATA_FIFO));
	terminal_writestring("\n");

	fdc_command(CONFIGURE);
	fdc_command(0);
	fdc_command(0x57);
	fdc_command(0);
	fdc_command(LOCK);
	timer_sleep(10);

	outb(0x00, DIGITAL_OUTPUT_REGISTER);	// disable controller
	timer_sleep(1);
	outb(0x1C, DIGITAL_OUTPUT_REGISTER);	// enable controller
	irq_wait(IRQ_FLOPPY);

	fdc_command(RECALIBRATE);
	fdc_command(0);
	irq_wait(IRQ_FLOPPY);
	fdc_command(RECALIBRATE);
	fdc_command(0);
	irq_wait(IRQ_FLOPPY);

	terminal_writestring("MSR = 0x");
	terminal_writebyte(inb(MAIN_STATUS_REGISTER));
	terminal_writestring("\n");
	terminal_writestring("DOR = 0x");
	terminal_writebyte(inb(DIGITAL_OUTPUT_REGISTER));
	terminal_writestring("\n");

	// fdc_read(0x0, 0x1000, 32);
	fdc_read(0x0, (uint32_t)FLOPPY_BUFFER, 32);

	dump(0x1000, 32);
	dump((uint32_t)FLOPPY_BUFFER, 32);
	terminal_writestring("dump done\n");

	while(1) {
		asm ("hlt\n");
	};

	asm (
		"cli\n"
		"hlt\n");
}

/******************************************************************************/
